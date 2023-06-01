import copy

# 不能有空格 不支持nop
INFILE = "mipscode.txt"
OUTFILE = INFILE.split('.')[0] + ".coe"

tag_dict = {}
r_dict = {'add', 'sub', 'and', 'or', 'xor', 'nor', 'sll', 'srl', 'sra', 'jr'}
i_dict = {'lw', 'sw', 'beq', 'bne', 'addi', 'andi', 'ori', 'xori', 'lui'}
j_dict = {'j', 'jal'}

special_r_dict = {'sll', 'srl', 'sra', 'jr'}
shift_r_dict = {'beq', 'bne', 'sll', 'srl', 'sra'}

special_i_dict = {'beq', 'bne', 'lw', 'sw', 'lui'}
branch_i_dict = {'beq', 'bne'}

op_code = {'addi': '001000',
           'andi': '001100',
           'ori': '001101',
           'xori': '001110',
           'lw': '100011',
           'sw': '101011',
           'beq': '000100',
           'bne': '000101',
           'lui': '001111',
           'j': '000010',
           'jal': '000011'}

funct = {'add': '100000',
         'sub': '100010',
         'and': '100100',
         'or': '100101',
         'xor': '100110',
         'nor': '100111',
         'sll': '000000',
         'srl': '000010',
         'sra': '000011',
         'jr': '001000'}

reg_dict = {
    '$0': 0,
    '$1': 1,
    '$2': 2,
    '$3': 3,
    '$4': 4,
    '$5': 5,
    '$6': 6,
    '$7': 7,
    '$8': 8,
    '$9': 9,
    '$10': 10,
    '$11': 11,
    '$12': 12,
    '$13': 13,
    '$14': 14,
    '$15': 15,
    '$16': 16,
    '$17': 17,
    '$18': 18,
    '$19': 19,
    '$20': 20,
    '$21': 21,
    '$22': 22,
    '$23': 23,
    '$24': 24,
    '$25': 25,
    '$26': 26,
    '$27': 27,
    '$28': 28,
    '$29': 29,
    '$30': 30,
    '$31': 31,
    '$at': 1,
    '$v0': 2,
    '$v1': 3,
    '$a0': 4,
    '$a1': 5,
    '$a2': 6,
    '$a3': 7,
    '$t0': 8,
    '$t1': 9,
    '$t2': 10,
    '$t3': 11,
    '$t4': 12,
    '$t5': 13,
    '$t6': 14,
    '$t7': 15,
    '$s0': 16,
    '$s1': 17,
    '$s2': 18,
    '$s3': 19,
    '$s4': 20,
    '$s5': 21,
    '$s6': 22,
    '$s7': 23,
    '$t8': 24,
    '$t9': 25,
    '$k0': 26,
    '$k1': 27,
    '$gp': 28,
    '$sp': 29,
    '$fp': 30,
    '$ra': 31
}


def complete(num, width):
    '''
    fill a string of length var<width> with binary representation of var<num>
    '''
    format_ = '{0:0>'+str(width)+'b}'
    if (num >= 0):
        return format_.format(num)
    else:
        return bin(num & int('1' * width, 2))


def bin2hex(bin_form):
    '''
    convert binary string to corresponding hexadecimal string
    '''
    format_ = '{0:1x}'
    hex_form = ''
    for i in range(0, 32, 4):
        hex_form += format_.format(int(bin_form[i: i+4], base=2))

    return hex_form


def reg2num(reg):
    '''
    convert register name to register number
    '''
    return reg_dict[reg.strip()]


def main():
    print(OUTFILE)
    in_file = open(INFILE, 'r')
    mips_ = in_file.readlines()
    in_file.close()

    out_file = open(OUTFILE, 'w')

    code = None
    machine_code = None
    op = None
    regs = None
    rs = None
    rt = None
    rd = None
    sa = None
    imm = None

    mips = copy.deepcopy(mips_)

    # clear comments
    for line in range(len(mips)):
        code = mips[line].strip().split('#')
        mips[line] = code[0].strip()

    # save tag
    for line in range(len(mips)):
        code = mips[line].strip().split(":")
        if (len(code) == 2):
            tag_dict[code[0].strip()] = line
            mips[line] = code[1].strip()
        elif (len(code) > 2):
            print("non-standard mips instruction encountered in line",
                  line, "code:", mips[line])
            exit()

    # conversion
    for line in range(len(mips)):
        code = mips[line].strip()
        whitespace_ind = code.find(' ')
        op = code[:whitespace_ind].strip()
        regs = code[whitespace_ind:].strip().split(',')

        # raw_input()
        print("------\ncode", code)
        print("whitepace_ind", whitespace_ind)
        print("op", op)
        print("regs", regs)
        # raw_input()

        machine_code = ''
        if op in r_dict:
            # 000000 $rs $rt $rd $shamt $funct
            machine_code += ('0' * 6)
            if op in special_r_dict:
                if op in shift_r_dict:
                    try:
                        rd = reg2num(regs[0])
                        rs = 0
                        rt = reg2num(regs[1])
                        sa = int(regs[2].strip())
                    except Exception as e:
                        print("Exception encountered in r-type conversion |", e)
                        # exit()
                else:  # jr
                    try:
                        rd = 0
                        rs = reg2num(regs[0])
                        rt = 0
                        sa = 0
                    except Exception as e:
                        print("Exception encountered in r-type conversion |", e)
                        # exit()
            else:
                try:
                    rd = reg2num(regs[0])
                    rs = reg2num(regs[1])
                    rt = reg2num(regs[2])
                    sa = 0
                except Exception as e:
                    print("Exception encountered in r-type conversion |", e)
                    # exit()
            print("rs:", rs, "type:", type(rs))
            print("rt:", rt, "type:", type(rt))
            print("rd:", rd, "type:", type(rd))
            print("sa:", sa, "type:", type(sa))
            machine_code += (complete(rs, 5) + complete(rt, 5) +
                             complete(rd, 5) + complete(sa, 5))
            machine_code += funct[op]

        elif op in i_dict:
            machine_code += op_code[op]
            if op in special_i_dict:
                if op in branch_i_dict:
                    try:
                        rs = reg2num(regs[0])
                        rt = reg2num(regs[1])
                        imm = tag_dict[regs[2].strip()] - line - 1
                    except Exception as e:
                        print("Exception encountered in i-type conversion |", e)
                elif op == 'lui':
                    try:
                        rs = 0
                        rt = reg2num(regs[0])
                        imm = int(regs[1].strip())
                    except Exception as e:
                        print("Exception encountered in i-type conversion |", e)
                else:  # sw lw
                    try:
                        rt = reg2num(regs[0])
                        regs[1] = regs[1].strip()
                        pos1 = regs[1].find('(')
                        pos2 = regs[1].find(')')
                        imm = int(regs[1][:pos1].strip())
                        rs = reg2num(regs[1][pos1+1:pos2])
                    except Exception as e:
                        print("Exception encountered in i-type conversion |", e)

            else:
                try:
                    rt = reg2num(regs[0])
                    rs = reg2num(regs[1])
                    imm = int(regs[2].strip())
                except Exception as e:
                    print("Exception encountered in i-type conversion |", e)
            machine_code += (complete(rs, 5)+complete(rt, 5)+complete(imm, 16))

        elif op in j_dict:
            machine_code += op_code[op]
            try:
                imm = int(regs[0].strip())
            except Exception as e:
                print("Exception encountered in j-type conversion |", e)
            machine_code += complete(imm, 26)

        else:
            print("unknown instruction encountered in line",
                  line, "code:", mips[line])
            exit()
        out_file.write(machine_code+',\n')
        # out_file.write(line_num_format.format(line) + ' : ' + bin2hex(machine_code) + ';  % ' + mips_[line].strip('\n') + ' | ' + machine_code + ' %' + '\n')

    out_file.close()


if __name__ == "__main__":
    main()
