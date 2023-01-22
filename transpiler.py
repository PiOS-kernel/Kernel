#! /usr/bin/python3

import sys


# S should be a '\n' terminated string

def get_token_literals(S):
    idx = 0
    output = []

    while S[idx] != '\n':
        # Skip whitespaces
        while S[idx] == ' ' or S[idx] == '\t':
            idx += 1

        # Check whether we are at the end of the line
        if S[idx] == '\n':
            return

        # Add the string to the output tokens buffer
        start = idx
        while S[idx] != ' ' and S[idx] != '\t':
            if S[idx] == '\n':
                output.append(S[start:idx])
                return output
            idx += 1

        output.append(S[start:idx])

    return output


# String literals only contain alphanumeric characters and '_'

def is_string_literal(S):
    for c in S:
        if (c < 'a' or c > 'z') and (c < 'A' or c > 'Z') and (c < '0' or c > '9') and c != '_':
            return False
        return True


# Constant declarations are of the form: 'constant_name' .word 'variable_name'

def is_constant_declaration(tokens):
    if len(tokens) < 3:
        return False

    return is_string_literal(tokens[0]) and tokens[1] == '.word' and is_string_literal(tokens[2])


# Labels are of the form: 'label_identifier':

def is_label(tokens):
    return len(tokens) >= 1 and is_string_literal(tokens[0][:-1]) and tokens[0][-1] == ':'


# General purpose registers labels are either of the form: 'r' + 'register_number' or
# one of ['sp', 'lr', 'pc']

def is_general_purpose_register(s):
    if len(s) != 2:
        return False
    
    if s[0] == 'r':
        try:
            reg_idx = int(s[1:])
            return reg_idx >= 0 and reg_idx <= 15
        except ValueError:
            return False
    else:
        return s == 'sp' or s == 'lr' or s == 'pc'


# Base class for the transpilers

class Transpiler:

    def __init__(self, filename):
        self.lines: list[str] = []
        self.output: list[str] = []
        self.idx = 0
        self.read_file(filename)

    # A line is added to the output buffer

    def write_line(self, line):
        self.output.append(line + "\n")

    def write(self, line):
        self.output.append(line)

    def current_line(self):
        return self.lines[self.idx]

    def has_next_line(self):
        return len(self.lines) - 1 > self.idx

    def next_line(self):
        self.idx += 1
        return self.current_line()

    def write_tokens(self, tokens):
        str_builder = ""
        for token in tokens:
            str_builder += token + " "
        self.write_line(str_builder)

    def skip_blank_lines(self):
        while self.has_next_line():
            curr_line = self.next_line()
            if curr_line != "\n":
                return

    def read_file(self, filename):
        with open(filename, "r") as f:
            lines = f.readlines()
            for line in lines:
                self.lines.append(line.strip() + "\n")

    def write_file(self, filename):
        with open(filename, "w") as f:
            for line in self.output:
                f.write(line)

    def transpile(self):
        assert False, "Not implemented"


class CCStoGCCTranspiler(Transpiler):

    def __init__(self, filename):
        super().__init__(filename)
        self.constants_map: dict[str][str] = {}

    # Just skips the initial section, until it gets to constant declarations

    def parse_header(self):
        if not self.has_next_line():
            return

        curr_line = self.current_line()
        while len(curr_line) > 0 and curr_line[0] == '.' and self.has_next_line():
            curr_line = self.next_line()

    # Populates the dictionary mapping local constants names to variable names

    def parse_constants(self):
        curr_line = self.current_line()
        curr_tokens = get_token_literals(curr_line)

        while is_constant_declaration(curr_tokens):
            self.constants_map[curr_tokens[0][:-1]] = curr_tokens[2]

            if not self.has_next_line():
                return
            curr_line = self.next_line()
            curr_tokens = get_token_literals(curr_line)

    def parse_instruction(self, tokens):
        # Replace instructions loading constants with '=m' pseudo instructions
        # Also translate comments
        for i in range(len(tokens)):
            if tokens[i] in self.constants_map:
                tokens[i] = "=" + self.constants_map[tokens[i]]
            elif tokens[i][0] == ";":
                tokens[i] = "@" + tokens[i][1:]

        str_builder = "    "
        for token in tokens:
            str_builder += token + " "
        return str_builder

    def parse_function(self, func_header_tokens):
        # Write the function header
        self.write_line(".thumb_func")
        self.write_line(".global " + func_header_tokens[0][:-1])
        self.write_line(func_header_tokens[0])

        # Translate instructions
        while self.has_next_line():
            curr_line = self.next_line()
            tokens = get_token_literals(curr_line)
            if len(tokens) > 0 and tokens[0] == ".endasmfunc":
                self.write_line("")
                return

            # Check whether the current line contains a label
            if is_label(tokens):
                self.write_tokens(tokens)
            else:
                self.write_line(self.parse_instruction(tokens))

    def parse_functions(self):
        curr_line = self.current_line()
        tokens = get_token_literals(curr_line)

        while self.has_next_line():
            # Skip blank lines
            while len(tokens) == 0:
                if not self.has_next_line():
                    return
                curr_line = self.next_line()
                tokens = get_token_literals(curr_line)

            # Translate comments
            if tokens[0][0] == ";":
                tokens[0] = "@" + tokens[0][1:]
                self.write_tokens(tokens)

            # Or translate function declaration
            else:
                self.parse_function(tokens)

            if not self.has_next_line():
                return
            curr_line = self.next_line()
            tokens = get_token_literals(curr_line)

    def transpile(self):
        # Write the header
        self.write_line(".cpu cortex-m3")
        self.write_line(".thumb")
        self.write_line(".syntax unified\n")
        self.write_line("@ -----------------------------------------------------------\n")

        self.skip_blank_lines()
        self.parse_header()
        self.skip_blank_lines()
        self.parse_constants()
        self.skip_blank_lines()
        self.parse_functions()


class GCCtoCCSTranspiler(Transpiler):

    def __init__(self, filename):
        super().__init__(filename)
        self.global_symbols = []
        self.external_references = []
        self.external_references_set = set()
        self.external_mappings = []

    # Adds an external reference to the dedicated sections in the header

    def add_external_reference(self, name):
        if name not in self.external_references_set:
            # .ref <var_name>
            self.external_references.append("    .ref " + name + "\n")

            # const<var_name>:         .word <var_name>
            s = "const" + name + ":        .word " + name + "\n"
            self.external_mappings.append(s)
            self.external_references_set.add(name)

        return "const" + name

    # Adds a global symbol to the dedicated section in the header

    def add_global_symbol(self, name):
        # .global <fun_name>
        self.global_symbols.append("    .global " + name + "\n")

    # Utility to dump hanging comments:
    def dump_hanging_comments(self, comments):
        if comments:
            self.write_line("")
            for comment in comments:
                self.write_tokens(comment)

    # Just skips the initial section

    def parse_header(self):
        if not self.has_next_line():
            return

        curr_line = self.current_line()
        while len(curr_line) > 0 and curr_line[0] == '.' and self.has_next_line():
            curr_line = self.next_line()

    def parse_instruction(self, tokens):
        # adds external references to the header when function calls are
        # encountered
        if tokens[0] in ["bl", "blx"] and not is_general_purpose_register(tokens[1]):
            self.external_references.append("    .ref " + tokens[1] + "\n")
        # Translates instructions loading variables into 
        # registers and translates comments
        else:
            for i in range(len(tokens)):
                if tokens[i][0] == "=":
                    tokens[i] = self.add_external_reference(tokens[i][1:])
                elif tokens[i][0] == "@":
                    tokens[i] = ";" + tokens[i][1:]

        str_builder = "    "
        for token in tokens:
            str_builder += token + " "
        return str_builder

    def translate_instruction(self, hanging_comments, tokens):
        # Dump the hanging comments
        for i in range(len(hanging_comments)):
            hanging_comments[i][0] = "    " + hanging_comments[i][0]
        self.dump_hanging_comments(hanging_comments)
        self.write_line(self.parse_instruction(tokens))

    def parse_function(self, func_header_tokens):
        # Write the function header
        self.write_line(func_header_tokens[0] + " .asmfunc")

        # Add the function symbol to the '.global' section
        self.add_global_symbol(func_header_tokens[0][:-1])

        hanging_comments = []
        # Translate instructions, until you reach an assembler directive, which
        # means that a new function declaration has begun
        while self.has_next_line():
            curr_line = self.next_line()
            tokens = get_token_literals(curr_line)
            if len(tokens) > 0 and tokens[0][0] == ".":
                self.write_line("\n    .endasmfunc\n")

                # If some comments were left hanging, dump them without indentation
                self.dump_hanging_comments(hanging_comments)
                return

            # Check whether if current line contains a label, if it does
            # make sure the label is something like:
            # <label>:
            #       <instruction>
            if is_label(tokens):
                if len(tokens) == 1:
                    self.write_tokens(tokens)
                else:
                    self.write_tokens(tokens[:1])
                    self.translate_instruction(hanging_comments, tokens[1:])
                    hanging_comments = []
            elif len(tokens) > 0:
                if tokens[0][0] == "@":
                    tokens[0] = ";" + tokens[0][1:]
                    hanging_comments.append(tokens)
                else:
                    self.translate_instruction(hanging_comments, tokens)
                    hanging_comments = []

        # This is the last function in the file
        self.write_line("\n    .endasmfunc\n")
        # If some comments were left hanging, dump them without indentation
        self.dump_hanging_comments(hanging_comments)

    def parse_functions(self):
        curr_line = self.current_line()
        tokens = get_token_literals(curr_line)

        while self.has_next_line():
            # Skip blank lines and assembler directives
            while len(tokens) == 0 or tokens[0][0] == ".":
                if not self.has_next_line():
                    return
                curr_line = self.next_line()
                tokens = get_token_literals(curr_line)

            # Translate comments
            if tokens[0][0] == "@":
                tokens[0] = ";" + tokens[0][1:]
                self.write_tokens(tokens)

            # Or translate function declaration
            else:
                self.parse_function(tokens)

            if not self.has_next_line():
                return
            curr_line = self.next_line()
            tokens = get_token_literals(curr_line)

    def transpile(self):
        self.skip_blank_lines()
        self.parse_header()
        self.skip_blank_lines()
        self.parse_functions()

        # Build the full output
        header = ["    .thumb\n"]
        for line in self.global_symbols:
            header.append(line)
        for line in self.external_references:
            header.append(line)

        header.append("\n")
        for line in self.external_mappings:
            header.append(line)

        self.output = header + self.output


def parse_CLI_args():
    if len(sys.argv) != 4 or sys.argv[3] != "gcc" and sys.argv[3] != "ccs":
        print("Arguments format:")
        print("    [1]. Input file path")
        print("    [2]. Output file path\n")
        print("    [3]. <gcc | ccs>")
        sys.exit(1)

    return sys.argv[1], sys.argv[2], sys.argv[3]


if __name__ == "__main__":
    input_file, output_file, target = parse_CLI_args()
    transpiler_cls = CCStoGCCTranspiler if target == "gcc" else GCCtoCCSTranspiler

    transpiler = transpiler_cls(input_file)
    transpiler.transpile()
    transpiler.write_file(output_file)
