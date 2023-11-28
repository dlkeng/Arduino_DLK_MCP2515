/*
 * NAME: commands.ino
 *
 * WHAT:
 *  Commands for serial command line I/F.
 *
 * SPECIAL CONSIDERATIONS:
 *  None
 *
 * AUTHOR:
 *  D.L. Karmann
 *
 * MODIFIED:
 *
 */

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// local function prototypes
int8_t Cmd_help(int8_t argc, char * argv[]);
int8_t Cmd_rreg(int8_t argc, char * argv[]);
int8_t Cmd_wreg(int8_t argc, char * argv[]);
int8_t Cmd_mode(int8_t argc, char * argv[]);
int8_t Cmd_canctrl(int8_t argc, char * argv[]);
int8_t Cmd_canstat(int8_t argc, char * argv[]);
int8_t Cmd_bfctrl(int8_t argc, char * argv[]);
int8_t Cmd_rtsctrl(int8_t argc, char * argv[]);
int8_t Cmd_tbctrl(int8_t argc, char * argv[]);
int8_t Cmd_rbctrl(int8_t argc, char * argv[]);
int8_t Cmd_send(int8_t argc, char * argv[]);
int8_t Cmd_xsend(int8_t argc, char * argv[]);
int8_t Cmd_scb(int8_t argc, char * argv[]);
int8_t Cmd_xscb(int8_t argc, char * argv[]);
int8_t Cmd_rtrsend(int8_t argc, char * argv[]);
int8_t Cmd_xrtrsnd(int8_t argc, char * argv[]);
int8_t Cmd_recv(int8_t argc, char * argv[]);
int8_t Cmd_stat(int8_t argc, char * argv[]);
int8_t Cmd_rxstat(int8_t argc, char * argv[]);
int8_t Cmd_init(int8_t argc, char * argv[]);
int8_t Cmd_inte(int8_t argc, char * argv[]);
int8_t Cmd_ints(int8_t argc, char * argv[]);
int8_t Cmd_errs(int8_t argc, char * argv[]);
int8_t Cmd_filt(int8_t argc, char * argv[]);
int8_t Cmd_xfilt(int8_t argc, char * argv[]);
int8_t Cmd_mask(int8_t argc, char * argv[]);
int8_t Cmd_xmask(int8_t argc, char * argv[]);
int8_t Cmd_rxmode(int8_t argc, char * argv[]);
int8_t Cmd_cnf(int8_t argc, char * argv[]);
int8_t Cmd_cmsg(int8_t argc, char * argv[]);
int8_t Cmd_dupe(int8_t argc, char * argv[]);
int8_t Cmd_cfg(int8_t argc, char * argv[]);

//*****************************************************************************
//
// These are the command names and brief descriptions.
//
// To add a menu item: (to keep all items in Flash)
//   1) add the command string to the 'MenuCmd#' item
//   2) add the command help string to the 'MenuHelp#' item
//   3) add the function prototype for the command's function above
//   4) add the 'MenuCmd#', function's name, and 'MenuHelp#' to the 'g_sCmdTable[]' array
//   5) add the function for processing the command to this file
//
//*****************************************************************************

// menu items individual command name strings
const char MenuCmdHelp1[] PROGMEM   = "help";
const char MenuCmdHelp2[] PROGMEM   = "h";
const char MenuCmdHelp3[] PROGMEM   = "?";
const char MenuCmdRreg[] PROGMEM    = "rreg";
const char MenuCmdWreg[] PROGMEM    = "wreg";
const char MenuCmdMode[] PROGMEM    = "mode";
const char MenuCmdCanctrl[] PROGMEM = "canctrl";
const char MenuCmdCanstat[] PROGMEM = "canstat";
const char MenuCmdBfctrl[] PROGMEM  = "bfctrl";
const char MenuCmdRtsctrl[] PROGMEM = "rtsctrl";
const char MenuCmdTbctrl[] PROGMEM  = "tbctrl";
const char MenuCmdRbctrl[] PROGMEM  = "rbctrl";
const char MenuCmdSend[] PROGMEM    = "send";
const char MenuCmdXsend[] PROGMEM   = "xsend";
const char MenuCmdScb[] PROGMEM     = "scb";
const char MenuCmdXscb[] PROGMEM    = "xscb";
const char MenuCmdRtrsend[] PROGMEM = "rtrsend";
const char MenuCmdXrtrsnd[] PROGMEM = "xrtrsnd";
const char MenuCmdRecv[] PROGMEM    = "recv";
const char MenuCmdStat[] PROGMEM    = "stat";
const char MenuCmdRxstat[] PROGMEM  = "rxstat";
const char MenuCmdInit[] PROGMEM    = "init";
const char MenuCmdInte[] PROGMEM    = "inte";
const char MenuCmdInts[] PROGMEM    = "ints";
const char MenuCmdErrs[] PROGMEM    = "errs";
const char MenuCmdFilt[] PROGMEM    = "filt";
const char MenuCmdXfilt[] PROGMEM   = "xfilt";
const char MenuCmdMask[] PROGMEM    = "mask";
const char MenuCmdXmask[] PROGMEM   = "xmask";
const char MenuCmdRxmode[] PROGMEM  = "rxmode";
const char MenuCmdCnf[] PROGMEM     = "cnf";
const char MenuCmdCmsg[] PROGMEM    = "cmsg";
const char MenuCmdDupe[] PROGMEM    = "dupe";
const char MenuCmdCfg[] PROGMEM     = "cfg";

// menu items individual command help strings
const char MenuHelp1[] PROGMEM       =    " [<cls>]                  : Display list of commands (clear screen)";
const char MenuHelp2[] PROGMEM       = "                             : alias for help";
const char MenuHelpRreg[] PROGMEM    =    " reg [cnt]                : Show MCP2515 register(s) value(s)";
const char MenuHelpWreg[] PROGMEM    =    " reg val                  : Set MCP2515 register value";
const char MenuHelpMode[] PROGMEM    =    " [cfg | norm | slp | lstn | loop] : Set/show MCP2515 mode {EEPROM}";
const char MenuHelpCanctrl[] PROGMEM =       "                       : Show MCP2515 CANCTRL register";
const char MenuHelpCanstat[] PROGMEM =       "                       : Show MCP2515 CANSTAT register";
const char MenuHelpBfctrl[] PROGMEM  =      " [bits]                 : Show[set] MCP2515 BFPCTRL register";
const char MenuHelpRtsctrl[] PROGMEM =       "                       : Show MCP2515 TXRTSCTRL register";
const char MenuHelpTbctrl[] PROGMEM  =      " [n]                    : Show MCP2515 TXBnCTRL register(s)";
const char MenuHelpRbctrl[] PROGMEM  =      " [n]                    : Show MCP2515 RXBnCTRL register(s)";
const char MenuHelpSend[] PROGMEM    =    " ID msg [cnt]             : Send CAN message";
const char MenuHelpXsend[] PROGMEM   =     " ID msg [cnt]            : Send Extended CAN message";
const char MenuHelpScb[] PROGMEM     =   " ID b1 [b2 ... b8]         : Send CAN byte(s)";
const char MenuHelpXscb[] PROGMEM    =    " ID b1 [b2 ... b8]        : Send Extended CAN byte(s)";
const char MenuHelpRtrsend[] PROGMEM =       " ID len                : Send Remote CAN frame";
const char MenuHelpXrtrsnd[] PROGMEM =       " ID len                : Send Extended Remote CAN frame";
const char MenuHelpRecv[] PROGMEM    =    "                          : Show received CAN message";
const char MenuHelpStat[] PROGMEM    =    "                          : Show MCP2515 Status";
const char MenuHelpRxstat[] PROGMEM  =      "                        : Show MCP2515 Rx Status";
const char MenuHelpInit[] PROGMEM    =    "                          : Reset and Init MCP2515";
const char MenuHelpInte[] PROGMEM    =    " [bits]                   : Show[set] MCP2515 interrupts enabled";
const char MenuHelpInts[] PROGMEM    =    " [bits | <clr>]           : Show[set | clr] MCP2515 interrupts";
const char MenuHelpErrs[] PROGMEM    =    " [<clr>]                  : Show MCP2515 errors (clear some)";
const char MenuHelpFilt[] PROGMEM    =    " [n ID D0 D1]             : Show[set] MCP2515 Filter registers {EEPROM}";
const char MenuHelpXfilt[] PROGMEM   =     " [n ID]                  : Show[set] MCP2515 Extended Filter regs {EEPROM}";
const char MenuHelpMask[] PROGMEM    =    " [n ID D0 D1]             : Show[set] MCP2515 Mask registers {EEPROM}";
const char MenuHelpXmask[] PROGMEM   =     " [n ID]                  : Show[set] MCP2515 Extended Mask registers {EEPROM}";
const char MenuHelpRxmode[] PROGMEM  =      " [n [<all> | <filt>]]   : Show[set] MCP2515 Rx mode(s)";
const char MenuHelpCnf[] PROGMEM     =   " [Kspd]                    : Show MCP2515 CNFn regs (set CAN speed {EEPROM})";
const char MenuHelpCmsg[] PROGMEM    =    " [<min> | <csv> | <norm>] : Show[set] CAN message format {EEPROM})";
const char MenuHelpDupe[] PROGMEM    =    " [<y> | <n>]              : Show[set] allow showing duped CAN messages {EEPROM}";
const char MenuHelpCfg[] PROGMEM     =   "                           : Show key configuration items";

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description. (Required by the 'CommandLine' command processor.)
//
//*****************************************************************************
const tCmdLineEntry g_sCmdTable[] PROGMEM =
{
    //  command       function     help info
    { MenuCmdHelp1,   Cmd_help,    MenuHelp1       },
    { MenuCmdHelp2,   Cmd_help,    MenuHelp2       },
    { MenuCmdHelp3,   Cmd_help,    MenuHelp2       },
    { MenuCmdRreg,    Cmd_rreg,    MenuHelpRreg    },
    { MenuCmdWreg,    Cmd_wreg,    MenuHelpWreg    },
    { MenuCmdMode,    Cmd_mode,    MenuHelpMode    },
    { MenuCmdCanctrl, Cmd_canctrl, MenuHelpCanctrl },
    { MenuCmdCanstat, Cmd_canstat, MenuHelpCanstat },
    { MenuCmdBfctrl,  Cmd_bfctrl,  MenuHelpBfctrl  },
    { MenuCmdRtsctrl, Cmd_rtsctrl, MenuHelpRtsctrl },
    { MenuCmdTbctrl,  Cmd_tbctrl,  MenuHelpTbctrl  },
    { MenuCmdRbctrl,  Cmd_rbctrl,  MenuHelpRbctrl  },
    { MenuCmdSend,    Cmd_send,    MenuHelpSend    },
    { MenuCmdXsend,   Cmd_xsend,   MenuHelpXsend   },
    { MenuCmdScb,     Cmd_scb,     MenuHelpScb     },
    { MenuCmdXscb,    Cmd_xscb,    MenuHelpXscb    },
    { MenuCmdRtrsend, Cmd_rtrsend, MenuHelpRtrsend },
    { MenuCmdXrtrsnd, Cmd_xrtrsnd, MenuHelpXrtrsnd },
    { MenuCmdRecv,    Cmd_recv,    MenuHelpRecv    },
    { MenuCmdStat,    Cmd_stat,    MenuHelpStat    },
    { MenuCmdRxstat,  Cmd_rxstat,  MenuHelpRxstat  },
    { MenuCmdInit,    Cmd_init,    MenuHelpInit    },
    { MenuCmdInte,    Cmd_inte,    MenuHelpInte    },
    { MenuCmdInts,    Cmd_ints,    MenuHelpInts    },
    { MenuCmdErrs,    Cmd_errs,    MenuHelpErrs    },
    { MenuCmdFilt,    Cmd_filt,    MenuHelpFilt    },
    { MenuCmdXfilt,   Cmd_xfilt,   MenuHelpXfilt   },
    { MenuCmdMask,    Cmd_mask,    MenuHelpMask    },
    { MenuCmdXmask,   Cmd_xmask,   MenuHelpXmask   },
    { MenuCmdRxmode,  Cmd_rxmode,  MenuHelpRxmode  },
    { MenuCmdCnf,     Cmd_cnf,     MenuHelpCnf     },
    { MenuCmdCmsg,    Cmd_cmsg,    MenuHelpCmsg    },
    { MenuCmdDupe,    Cmd_dupe,    MenuHelpDupe    },
    { MenuCmdCfg,     Cmd_cfg,     MenuHelpCfg     },
    { 0, 0, 0 }     // end of commands
};

/*
 * NAME:
 *  int8_t Cmd_rreg(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "rreg" command to read and show an MCP2515 register.
 *
 *  One required parameter supported.
 *   reg = the register(s) to read from
 *  One optional parameter supported.
 *   cnt = the number of register(s) to read from
 *
 *        1   2    3
 *     "rreg 0x40"      - read MCP2515 register 0x40
 *     "rreg 0x40 10"   - read MCP2515 registers 0x40 to 0x49
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_rreg(int8_t argc, char * argv[])
{
    int32_t val;
    int8_t paramtype;
    uint8_t regaddr;
    uint8_t regcnt = 1;
    uint8_t regval;

    if (argc < 2)
    {
        return CMDLINE_TOO_FEW_ARGS;
    }
    else if (argc > 3)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else
    {
        // get the register address
        paramtype = CmdLine.ParseParam(argv[ARG1], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 0) || (val > MCP2515_RXB1D7))
        {
            return CMDLINE_INVALID_ARG;
        }
        regaddr = val;
        if (argc > 2)
        {
            // get the count of registers to read from
            paramtype = CmdLine.ParseParam(argv[ARG2], &val);
            if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
                (val < 1) || (val > 20))
            {
                return CMDLINE_INVALID_ARG;
            }
            regcnt = val;
        }

        Serial.print(F("MCP2515 Register "));
        Print0xHexByte(regaddr);
        Serial.print(F(": "));
        for (uint8_t i = 0; i < regcnt; ++i)
        {
            regval = Mcp2515.MCP2515_ReadRegister(regaddr + i);
            Print0xHexByte(regval);
            Serial.print(' ');
        }
       Serial.println();
    }

    // Return success.
    return 0;
}

/*
 * NAME:
 *  int8_t Cmd_wreg(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "wreg" command to write to an MCP2515 register.
 *
 *  Two required parameters supported.
 *   reg = the register to write to
 *   val = the value to write to the register
 *
 *        1   2    3
 *     "wreg 0x40 0xaa"     - read MCP2515 register 0x40 with value 0xaa
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_wreg(int8_t argc, char * argv[])
{
    int32_t val;
    int8_t paramtype;
    uint8_t regaddr;
    uint8_t regval;

    if (argc < 3)
    {
        return CMDLINE_TOO_FEW_ARGS;
    }
    else if (argc > 3)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else
    {
        // get the register address
        paramtype = CmdLine.ParseParam(argv[ARG1], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 0) || (val > MCP2515_RXB1D7))
        {
            return CMDLINE_INVALID_ARG;
        }
        regaddr = val;

        // get the value to write
        paramtype = CmdLine.ParseParam(argv[ARG2], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 0) || (val > 0xff))
        {
            return CMDLINE_INVALID_ARG;
        }
        regval = val;

        if (Mcp2515.MCP2515_CheckRegisterWritable(regaddr) != MCP2515_OK)
        {
            Serial.print(F("Register "));
            Print0xHexByte(regaddr);
            Serial.println(F(" not writable in non-Configuration mode!"));
        }
        else
        {
            // write the value to the register
            Mcp2515.MCP2515_WriteRegister(regaddr, regval);
        }
    }

    // Return success.
    return 0;
}

/*
 * NAME:
 *  int8_t Cmd_mode(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "mode" command to set/show MCP2515 mode.
 *
 *  One optional parameter supported.
 *   <mode> = MCP2515 mode ("cfg" or "norm" or "slp" or "lstn" or "loop")
 *                         (config,   normal,   sleep,   listen,   loop-back)
 *
 *        1   2
 *     "mode"           - show MCP2515 mode
 *     "mode cfg"       - set MCP2515 to Configuration mode
 *     "mode norm"      - set MCP2515 to Normal mode
 *     "mode slp"       - set MCP2515 to Sleep mode
 *     "mode lstn"      - set MCP2515 to Listen mode
 *     "mode loop"      - set MCP2515 to Loop-back mode
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  Setting the MCP2515 CAN mode is saved in EEPROM.
 */
int8_t Cmd_mode(int8_t argc, char * argv[])
{
    uint8_t rslt;
    uint8_t mode;

    if (argc < 2)
    {
        // OK, just show current mode
    }
    else if (argc > 2)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else
    {
        if (strcmp_P(argv[ARG1], PSTR("cfg")) == 0)
        {
            mode = MODE_CONFIG;
        }
        else if (strcmp_P(argv[ARG1], PSTR("norm")) == 0)
        {
            mode = MODE_NORMAL;
        }
        else if (strcmp_P(argv[ARG1], PSTR("slp")) == 0)
        {
            mode = MODE_SLEEP;
        }
        else if (strcmp_P(argv[ARG1], PSTR("lstn")) == 0)
        {
            mode = MODE_LISTENONLY;
        }
        else if (strcmp_P(argv[ARG1], PSTR("loop")) == 0)
        {
            mode = MODE_LOOPBACK;
        }
        else    // invalid mode
        {
            return CMDLINE_INVALID_ARG;
        }

        rslt = Mcp2515.MCP2515_SetMode(mode);
        if (rslt != MCP2515_OK)
        {
            return rslt;
        }
        CAN_Mode = mode;
#if defined(ESP8266) || defined(ESP32)
        EEPROM.write(EE_CAN_MODE, CAN_Mode);
#else
        EEPROM.update(EE_CAN_MODE, CAN_Mode);
#endif
        CommitEEPROM();
    }

    mode = Mcp2515.MCP2515_ReadRegister(MCP2515_CANCTRL) & MODE_MASK;
    Serial.print(F(" Mode: "));
    ShowCAN_Mode(mode);

   // Return success.
    return 0;
}

// show MCP2515 CAN mode setting
void ShowCAN_Mode(uint8_t mode)
{
    switch (mode)
    {
        case MODE_NORMAL:
            Serial.println(F("Normal"));
            break;
        case MODE_SLEEP:
            Serial.println(F("Sleep"));
            break;
        case MODE_LOOPBACK:
            Serial.println(F("Loop-back"));
            break;
        case MODE_LISTENONLY:
            Serial.println(F("Listen-only"));
            break;
        case MODE_CONFIG:
            Serial.println(F("Configuration"));
            break;
        default:
            Serial.print(F("Unknown ("));
            Print0xHexByte(mode);
            Serial.println(')');
            break;
    }
}

/*
 * NAME:
 *  int8_t Cmd_canctrl(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "canctrl" command to show MCP2515 CANCTRL register.
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_canctrl(int8_t argc __attribute__((unused)), char * argv[] __attribute__((unused)))
{
    ShowCANCTRL_Reg();     // show MCP2515 CANCTRL register

    // Return success.
    return 0;
}

// get and show MCP2515 CANCTRL register
void ShowCANCTRL_Reg(void)
{
    uint8_t reg;
    uint8_t cnf3;

    reg = Mcp2515.MCP2515_ReadRegister(MCP2515_CANCTRL);
    cnf3 = Mcp2515.MCP2515_ReadRegister(MCP2515_CNF3);

    Serial.print(F("CANCTRL("));
    Print0xHexByte(MCP2515_CANCTRL);
    Serial.print(F("): "));
    Print0xHexByte(reg);
    Serial.println();

    Serial.print(F(" - [7:5]REQOP[2:0]:  "));
    ShowCAN_Mode(reg & MODE_MASK);

    Serial.print(F(" - [4]ABAT:          "));
    Serial.print(F("Abort "));
    if (reg & ABORT_TX)
    {
        Serial.print(F("pending"));
    }
    else
    {
        Serial.print(F("all"));
    }
    Serial.println(F(" transmissions"));

    Serial.print(F(" - [3]OSM:           "));
    Serial.print(F("One-shot "));
    if (reg & MODE_ONESHOT)
    {
        Serial.println(F("Enabled"));
    }
    else
    {
        Serial.println(F("Disabled"));
    }

    Serial.print(F(" - [2]CLKEN:         "));
    Serial.print(F("CLKOUT pin "));
    if (reg & CLKOUT_ENABLE)
    {
        Serial.println(F("Enabled"));
    }
    else
    {
        Serial.println(F("Disabled"));
    }

    Serial.print(F(" - [1:0]CLKPRE[1:0]: "));
    if (reg & CLKOUT_ENABLE)
    {
        if (cnf3 & SOF_ENABLE)
        {
            Serial.println(F("SOF Signal"));
            return;
        }
    }

    switch (reg & CLKOUT_MASK)
    {
        case CLKOUT_PS1:                // System Clock/1
            Serial.println(F("System Clock/1"));
            break;
        case CLKOUT_PS2:                // System Clock/2
            Serial.println(F("System Clock/2"));
            break;
        case CLKOUT_PS4:                // System Clock/4
            Serial.println(F("System Clock/4"));
            break;
        case CLKOUT_PS8:                // System Clock/8
            Serial.println(F("System Clock/8"));
            break;
    }
}

/*
 * NAME:
 *  int8_t Cmd_canstat(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "canstat" command to show MCP2515 CANSTAT register.
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_canstat(int8_t argc __attribute__((unused)), char * argv[] __attribute__((unused)))
{
    ShowCANSTAT_Reg();     // show MCP2515 CANSTAT register

    // Return success.
    return 0;
}

// get and show MCP2515 CANSTAT register
void ShowCANSTAT_Reg(void)
{
    uint8_t reg;

    reg = Mcp2515.MCP2515_ReadRegister(MCP2515_CANSTAT);

    Serial.print(F("CANSTAT("));
    Print0xHexByte(MCP2515_CANSTAT);
    Serial.print(F("): "));
    Print0xHexByte(reg);
    Serial.println();

    Serial.print(F(" - [7:5]OPMOD[2:0]: "));
    ShowCAN_Mode(reg & MODE_MASK);

    Serial.print(F(" - [3:1]ICOD[2:0]:  "));
    switch (reg & INT_MASK)
    {
        case NO_INT:
            Serial.print(F("No"));
            break;
        case ERROR_INT:
            Serial.print(F("Error"));
            break;
        case WAKE_UP_INT:
            Serial.print(F("Wake-up"));
            break;
        case TXB0_INT:
            Serial.print(F("TXB0"));
            break;
        case TXB1_INT:
            Serial.print(F("TXB1"));
            break;
        case TXB2_INT:
            Serial.print(F("TXB2"));
            break;
        case RXB0_INT:
            Serial.print(F("RXB0"));
            break;
        case RXB1_INT:
            Serial.print(F("RXB1"));
            break;
    }
    Serial.println(F(" interrupt"));
}

/*
 * NAME:
 *  int8_t Cmd_bfctrl(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "bfctrl" command to show MCP2515 BFPCTRL register.
 *
 *  One optional parameter supported.
 *   bits = sets/clears BFPCTRL RXnBF pins enabled/disabled/mode/state
 *
 *        1     2 
 *     "bfctrl"         - show MCP2515 BFPCTRL register
 *     "bfctrl 0x1a"    - set MCP2515 BFPCTRL register to 0x1a
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_bfctrl(int8_t argc, char * argv[])
{
    int32_t val;
    int8_t paramtype;
    uint8_t regval;

    if (argc < 2)
    {
        // OK, just show MCP2515 BFPCTRL register
    }
    else if (argc > 2)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else
    {
        // get the register value
        paramtype = CmdLine.ParseParam(argv[ARG1], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 0) || (val > 0x3f))
        {
            return CMDLINE_INVALID_ARG;
        }
        regval = val;
        Mcp2515.MCP2515_WriteRegister(MCP2515_BFPCTRL, regval);
    }

    ShowBFPCTRL_Reg();     // show MCP2515 BFPCTRL register

    // Return success.
    return 0;
}

// show MCP2515 BFPCTRL register
void ShowBFPCTRL_Reg(void)
{
    uint8_t reg;

    reg = Mcp2515.MCP2515_ReadRegister(MCP2515_BFPCTRL);

    Serial.print(F("BFPCTRL("));
    Print0xHexByte(MCP2515_BFPCTRL);
    Serial.print(F("): "));
    Print0xHexByte(reg);
    Serial.println();

    Serial.print(F(" - [5]B1BFS: "));
    Serial.print(F("RX1BF Pin State = "));
    if (reg & B1BFS_BIT)
    {
        Serial.println('1');
    }
    else
    {
        Serial.println('0');
    }

    Serial.print(F(" - [4]B0BFS: "));
    Serial.print(F("RX0BF Pin State = "));
    if (reg & B0BFS_BIT)
    {
        Serial.println('1');
    }
    else
    {
        Serial.println('0');
    }

    Serial.print(F(" - [3]B1BFE: "));
    Serial.print(F("RX1BF Pin Function "));
    if (reg & B1BFE_BIT)
    {
        Serial.println(F("Enabled"));
    }
    else
    {
        Serial.println(F("Disabled"));
    }

    Serial.print(F(" - [2]B0BFE: "));
    Serial.print(F("RX0BF Pin Function "));
    if (reg & B0BFE_BIT)
    {
        Serial.println(F("Enabled"));
    }
    else
    {
        Serial.println(F("Disabled"));
    }

    Serial.print(F(" - [1]B1BFM: "));
    Serial.print(F("RX1BF Pin Mode "));
    if (reg & B1BFM_BIT)
    {
        Serial.println(F("RXB1 interrupt"));
    }
    else
    {
        Serial.println(F("Digital output"));
    }

    Serial.print(F(" - [0]B0BFM: "));
    Serial.print(F("RX0BF Pin Mode "));
    if (reg & B0BFM_BIT)
    {
        Serial.println(F("RXB0 interrupt"));
    }
    else
    {
        Serial.println(F("Digital output"));
    }
}

/*
 * NAME:
 *  int8_t Cmd_rtsctrl(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "rtsctrl" command to show MCP2515 TXRTSCTRL register.
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_rtsctrl(int8_t argc __attribute__((unused)), char * argv[] __attribute__((unused)))
{
    ShowTXRTSCTRL_Reg();    // show MCP2515 TXRTSCTRL register

    // Return success.
    return 0;
}

// show MCP2515 TXRTSCTRL register
void ShowTXRTSCTRL_Reg(void)
{
    uint8_t reg;

    reg = Mcp2515.MCP2515_ReadRegister(MCP2515_TXRTSCTRL);

    Serial.print(F("TXRTSCTRL("));
    Print0xHexByte(MCP2515_TXRTSCTRL);
    Serial.print(F("): "));
    Print0xHexByte(reg);
    Serial.println();

    Serial.print(F(" - [5]B2RTS:  "));
    Serial.print(F("TX2RTS Pin State = "));
    if (reg & B2RTS_BIT)
    {
        Serial.println('1');
    }
    else
    {
        Serial.println('0');
    }

    Serial.print(F(" - [3]B1RTS:  "));
    Serial.print(F("TX1RTS Pin State = "));
    if (reg & B1RTS_BIT)
    {
        Serial.println('1');
    }
    else
    {
        Serial.println('0');
    }

    Serial.print(F(" - [3]B0RTS:  "));
    Serial.print(F("TX0RTS Pin State = "));
    if (reg & B0RTS_BIT)
    {
        Serial.println('1');
    }
    else
    {
        Serial.println('0');
    }

    Serial.print(F(" - [2]B2RTSM: "));
    Serial.print(F("TX2RTS Pin Mode "));
    if (reg & B2RTSM_BIT)
    {
        Serial.println(F("TXB2 buffer transmit"));
    }
    else
    {
        Serial.println(F("Digital input"));
    }

    Serial.print(F(" - [1]B1RTSM: "));
    Serial.print(F("TX1RTS Pin Mode "));
    if (reg & B1RTSM_BIT)
    {
        Serial.println(F("TXB1 buffer transmit"));
    }
    else
    {
        Serial.println(F("Digital input"));
    }

    Serial.print(F(" - [0]B0RTSM: "));
    Serial.print(F("TX0RTS Pin Mode "));
    if (reg & B0RTSM_BIT)
    {
        Serial.println(F("TXB0 buffer transmit"));
    }
    else
    {
        Serial.println(F("Digital input"));
    }
}

/*
 * NAME:
 *  int8_t Cmd_tbctrl(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "tbctrl" command to show MCP2515 TXBnCTRL register(s).
 *
 *  One optional parameter supported.
 *   n = the TXBnCTRL register to show (0 or 1 or 2)
 *
 *        1    2 
 *     "tbctrl"     - show MCP2515 TXBnCTRL registers
 *     "tbctrl 0"   - show MCP2515 TXB0CTRL register
 *     "tbctrl 1"   - show MCP2515 TXB1CTRL register
 *     "tbctrl 2"   - show MCP2515 TXB2CTRL register
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_tbctrl(int8_t argc, char * argv[])
{
    int32_t val;
    int8_t paramtype;
    uint8_t tb_num;

    if (argc < 2)
    {
        for (uint8_t tb_num = TXB0; tb_num <= TXB2; ++tb_num)
        {
            ShowTXBnCTRL_Reg(tb_num);    // show MCP2515 TXBnCTRL register
        }
    }
    else if (argc > 2)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else
    {
        // get the transmit buffer number
        paramtype = CmdLine.ParseParam(argv[ARG1], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 0) || (val > 2))
        {
            return CMDLINE_INVALID_ARG;
        }
        tb_num = val;

        ShowTXBnCTRL_Reg(tb_num);    // show MCP2515 TXBnCTRL register
    }

    // Return success.
    return 0;
}

// show MCP2515 TXBnCTRL register
void ShowTXBnCTRL_Reg(uint8_t tb_num)
{
    const uint8_t ctrlregs[MCP2515_N_TXBUFFERS] = { MCP2515_TXB0CTRL, MCP2515_TXB1CTRL, MCP2515_TXB2CTRL };
    uint8_t reg;

    reg = Mcp2515.MCP2515_ReadRegister(ctrlregs[tb_num]);

    Serial.print(F("TXB"));
    Serial.print(tb_num);
    Serial.print(F("CTRL("));
    Print0xHexByte(ctrlregs[tb_num]);
    Serial.print(F("): "));
    Print0xHexByte(reg);
    Serial.println();

    Serial.print(F(" - [6]ABTF:       "));
    Serial.print(F("Message Aborted Flag     = "));
    Serial.print(F("Message "));
    if (reg & TXB_ABTF_BIT)
    {
        Serial.println(F("aborted"));
    }
    else
    {
        Serial.println(F("completed"));
    }

    Serial.print(F(" - [5]MLOA:       "));
    Serial.print(F("Message Lost Arbitration = "));
    Serial.print(F("Message "));
    if (reg & TXB_MLOA_BIT)
    {
        Serial.print(F("lost"));
    }
    else
    {
        Serial.print(F("did not lose"));
    }
    Serial.println(F(" arbitration"));

    Serial.print(F(" - [4]TXERR:      "));
    Serial.print(F("Transmission Error       = "));
    if (reg & TXB_TXERR_BIT)
    {
        Serial.print('B');
    }
    else
    {
        Serial.print(F("No b"));
    }
    Serial.println(F("us error occurred"));

    Serial.print(F(" - [3]TXREQ:      "));
    Serial.print(F("Transmit Request         = "));
    if (reg & TXB_TXREQ_BIT)
    {
    }
    else
    {
        Serial.print(F("Not "));
    }
    Serial.println(F("Pending transmission"));

    Serial.print(F(" - [1:0]TXP[1:0]: "));
    Serial.print(F("Buffer Priority          = "));
    switch (reg & TXP_MASK)
    {
        case TXP_P3:
            Serial.print(F("Highest"));
            break;
        case TXP_P2:
            Serial.print(F("High intermediate"));
            break;
        case TXP_P1:
            Serial.print(F("Low intermediate"));
            break;
        case TXP_P0:
            Serial.print(F("Lowest"));
            break;
    }
    Serial.println(F(" priority"));
}

/*
 * NAME:
 *  int8_t Cmd_rbctrl(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "rbctrl" command to show MCP2515 RXBnCTRL register(s).
 *
 *  One optional parameter supported.
 *   n = the RXBnCTRL register to show (0 or 1)
 *
 *        1    2 
 *     "rbctrl"     - show MCP2515 RXBnCTRL registers
 *     "rbctrl 0"   - show MCP2515 RXB0CTRL register
 *     "rbctrl 1"   - show MCP2515 RXB1CTRL register
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_rbctrl(int8_t argc, char * argv[])
{
    int32_t val;
    int8_t paramtype;
    uint8_t rb_num;

    if (argc < 2)
    {
        for (uint8_t rb_num = RXB0; rb_num <= RXB1; ++rb_num)
        {
            ShowRXBnCTRL_Reg(rb_num);    // show MCP2515 RXBnCTRL register
        }
    }
    else if (argc > 2)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else
    {
        // get the receive buffer number
        paramtype = CmdLine.ParseParam(argv[ARG1], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 0) || (val > 1))
        {
            return CMDLINE_INVALID_ARG;
        }
        rb_num = val;

        ShowRXBnCTRL_Reg(rb_num);    // show MCP2515 RXBnCTRL register
    }

    // Return success.
    return 0;
}

// show MCP2515 RXBnCTRL register
void ShowRXBnCTRL_Reg(uint8_t rb_num)
{
    const uint8_t ctrlregs[MCP2515_N_TXBUFFERS] = { MCP2515_RXB0CTRL, MCP2515_RXB1CTRL };
    uint8_t reg;

    reg = Mcp2515.MCP2515_ReadRegister(ctrlregs[rb_num]);

    Serial.print(F("RXB"));
    Serial.print(rb_num);
    Serial.print(F("CTRL("));
    Print0xHexByte(ctrlregs[rb_num]);
    Serial.print(F("): "));
    Print0xHexByte(reg);
    Serial.println();

    Serial.print(F(" - [6:5]RXM[1:0]: Buffer Operating mode = "));
    Serial.print(F("Receives "));
    switch (reg & RXM_MASK)
    {
        case RXM_M3:
            Serial.println(F("any message"));
            break;
        case RXM_M0:
            Serial.println(F("all valid messages"));
            break;
    }

    Serial.print(F(" - [3]RXRTR:      "));
    Serial.print(F("Received Remote Transfer Request = "));
    if (reg & RXRTR_BIT)
    {
    }
    else
    {
        Serial.print(F("No "));
    }
    Serial.println(F("RTR received"));

    if (rb_num == 0)
    {
        Serial.print(F(" - [2]BUKT:       "));
        Serial.print(F("Rollover Enable = "));
        if (reg & BUKT_BIT)
        {
            Serial.println(F("enabled"));
        }
        else
        {
            Serial.println(F("disabled"));
        }

        Serial.print(F(" - [1]BUKT1:      "));
        Serial.print(F("Rollover Enable copy = "));
        if (reg & BUKT1_BIT)
        {
            Serial.println(F("enabled"));
        }
        else
        {
            Serial.println(F("disabled"));
        }

        Serial.print(F(" - [0]FILHIT0:    "));
        Serial.print(F("Filter Hit = RXF"));
        Serial.println(reg & FILHIT0_BIT);
    }
    else if (rb_num == 1)
    {
        Serial.print(F(" - [2:0]FILHIT[2:0]: "));
        Serial.print(F("Filter Hit = RXF"));
        Serial.println(reg & FILHIT_MASK);
    }
}

// check for correct sending mode
int8_t CheckSendingMode(void)
{
    uint8_t mode;

    mode = Mcp2515.MCP2515_ReadRegister(MCP2515_CANCTRL) & MODE_MASK;
    if ((mode != MODE_NORMAL) && (mode != MODE_LOOPBACK))
    {
        Serial.print(F("Incorrect mode for sending: "));
        ShowCAN_Mode(mode);
        return MCP2515_FAIL;
    }
    return MCP2515_OK;
}

// check for correct sending response
int8_t CheckSendingResponse(uint8_t resp)
{
    if (resp != MCP2515_OK)
    {
        Serial.println(F("No CAN device responding on CAN bus!"));
        Serial.println(F("Check CAN bus connections or CAN bus speed!"));
    }
    return resp;
}

/*
 * NAME:
 *  int8_t Cmd_send(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "send" command to send Standard CAN message.
 *
 *  Two required parameters supported.
 *   ID = the CAN ID to send
 *   msg = the CAN message to send
 *  One optional parameter supported.
 *   cnt = the number of times to send message
 *
 *        1   2    3      4
 *     "send 0x123 hello"      - send Standard CAN message with ID 0x123 and text data "hello"
 *     "send 0x123 hello  5"   - send Standard CAN message with ID 0x123 and text data "hello" 5 times
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_send(int8_t argc, char * argv[])
{
    int32_t val;
    int8_t paramtype;
    uint16_t id_val;
    uint8_t cnt = 1;
    uint8_t len;

    if (argc < 3)
    {
        return CMDLINE_TOO_FEW_ARGS;
    }
    else if (argc > 4)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else
    {
        len = strlen(argv[ARG2]);
        if (len > CAN_MAX_MESSAGE_LENGTH)
        {
            return CMDLINE_INVALID_ARG;
        }
    }

    // get the ID to send
    paramtype = CmdLine.ParseParam(argv[ARG1], &val);
    if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
        (val < 0) || ((uint32_t)val > CAN_SFF_MASK))
    {
        return CMDLINE_INVALID_ARG;
    }
    id_val = val;

    if (argc > 3)
    {
        // get the count of times to send message
        paramtype = CmdLine.ParseParam(argv[ARG3], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 1) || (val > 20))
        {
            return CMDLINE_INVALID_ARG;
        }
        cnt = val;
    }

    if (CheckSendingMode() == MCP2515_FAIL)
    {
        return MCP2515_FAIL;
    }

    for (uint8_t i = 0; i < cnt; ++i)
    {
        if (CheckSendingResponse(Mcp2515.MCP2515_Send(id_val, len, (uint8_t *)argv[ARG2])) != MCP2515_OK)
        {
            return MCP2515_FAIL;
        }
    }

    // Return success.
    return 0;
}

/*
 * NAME:
 *  int8_t Cmd_xsend(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "send" command to send Extended CAN message.
 *
 *  Two required parameters supported.
 *   ID = the CAN ID to send
 *   msg = the CAN message to send
 *  One optional parameter supported.
 *   cnt = the number of times to send message
 *
 *         1   2    3      4
 *     "xsend 0x123 hello"      - send Extended CAN message with ID 0x123 and text data "hello"
 *     "xsend 0x123 hello  5"   - send Extended CAN message with ID 0x123 and text data "hello" 5 times
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_xsend(int8_t argc, char * argv[])
{
    int32_t val;
    int8_t paramtype;
    uint32_t id_val;
    uint8_t cnt = 1;
    uint8_t len;

    if (argc < 3)
    {
        return CMDLINE_TOO_FEW_ARGS;
    }
    else if (argc > 4)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else
    {
        len = strlen(argv[ARG2]);
        if (len > CAN_MAX_MESSAGE_LENGTH)
        {
            return CMDLINE_INVALID_ARG;
        }
    }

    // get the ID to send
    paramtype = CmdLine.ParseParam(argv[ARG1], &val);
    if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
        (val < 0) || ((uint32_t)val > CAN_EFF_MASK))
    {
        return CMDLINE_INVALID_ARG;
    }
    id_val = val;

    if (argc > 3)
    {
        // get the count of times to send message
        paramtype = CmdLine.ParseParam(argv[ARG3], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 1) || (val > 20))
        {
            return CMDLINE_INVALID_ARG;
        }
        cnt = val;
    }

    if (CheckSendingMode() == MCP2515_FAIL)
    {
        return MCP2515_FAIL;
    }

    for (uint8_t i = 0; i < cnt; ++i)
    {
        if (CheckSendingResponse(Mcp2515.MCP2515_Xsend(id_val, len, (uint8_t *)argv[ARG2])) != MCP2515_OK)
        {
            return MCP2515_FAIL;
        }
    }

    // Return success.
    return 0;
}

/*
 * NAME:
 *  int8_t Cmd_scb(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "scb" command to send Standard CAN byte(s).
 *
 *  Two required parameters supported.
 *   ID = the CAN ID to send
 *   b1 = a CAN byte to send
 *  7 optional parameters supported.
 *   b2 ... b8 = additional CAN bytes to send
 *
 *       1   2     3   4 5 6 7 8 9 10
 *     "scb 0x123 0x5a"                 - send Standard CAN message with ID 0x123 and byte data 0x5a
 *     "scb 0x123 0xa5 5 2 1 6 7 0 4"   - send Standard CAN message with ID 0x123 and byte data 0xa5 5 2 1 6 7 0 4
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_scb(int8_t argc, char * argv[])
{
    int32_t val;
    int8_t paramtype;
    uint16_t id_val;
    uint8_t data[8];
    uint8_t bcnt = 0;

    if (argc < 3)
    {
        return CMDLINE_TOO_FEW_ARGS;
    }
    else if (argc > 10)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }

    // get the ID to send
    paramtype = CmdLine.ParseParam(argv[ARG1], &val);
    if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
        (val < 0) || ((uint32_t)val > CAN_SFF_MASK))
    {
        return CMDLINE_INVALID_ARG;
    }
    id_val = val;

    for (uint8_t i = 0; i < (argc - ARG2); ++i)
    {
        // get the bytes to send
        paramtype = CmdLine.ParseParam(argv[ARG2 + i], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 0) || (val > 0xff))
        {
            return CMDLINE_INVALID_ARG;
        }
        data[i] = val;
        ++bcnt;
    }

    if (CheckSendingMode() == MCP2515_FAIL)
    {
        return MCP2515_FAIL;
    }

    if (CheckSendingResponse(Mcp2515.MCP2515_Send(id_val, bcnt, data)) != MCP2515_OK)
    {
        return MCP2515_FAIL;
    }

    // Return success.
    return 0;
}

/*
 * NAME:
 *  int8_t Cmd_xscb(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "xscb" command to send Extended CAN byte(s).
 *
 *  Two required parameters supported.
 *   ID = the CAN ID to send
 *   b1 = a CAN byte to send
 *  7 optional parameters supported.
 *   b2 ... b8 = additional CAN bytes to send
 *
 *       1   2      3   4 5 6 7 8 9 10
 *     "xscb 0x123 0x5a"                 - send Extended CAN message with ID 0x123 and byte data 0x5a
 *     "xscb 0x123 0xa5 5 2 1 6 7 0 4"   - send Extended CAN message with ID 0x123 and byte data 0xa5 5 2 1 6 7 0 4
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_xscb(int8_t argc, char * argv[])
{
    int32_t val;
    int8_t paramtype;
    uint16_t id_val;
    uint8_t data[8];
    uint8_t bcnt = 0;

    if (argc < 3)
    {
        return CMDLINE_TOO_FEW_ARGS;
    }
    else if (argc > 10)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }

    // get the ID to send
    paramtype = CmdLine.ParseParam(argv[ARG1], &val);
    if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
        (val < 0) || ((uint32_t)val > CAN_SFF_MASK))
    {
        return CMDLINE_INVALID_ARG;
    }
    id_val = val;

    for (uint8_t i = 0; i < (argc - ARG2); ++i)
    {
        // get the bytes to send
        paramtype = CmdLine.ParseParam(argv[ARG2 + i], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 0) || (val > 0xff))
        {
            return CMDLINE_INVALID_ARG;
        }
        data[i] = val;
        ++bcnt;
    }

    if (CheckSendingMode() == MCP2515_FAIL)
    {
        return MCP2515_FAIL;
    }

    if (CheckSendingResponse(Mcp2515.MCP2515_Xsend(id_val, bcnt, data)) != MCP2515_OK)
    {
        return MCP2515_FAIL;
    }

    // Return success.
    return 0;
}

/*
 * NAME:
 *  int8_t Cmd_rtrsend(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "rtrsend" command to send Standard Remote CAN frame.
 *
 *  Two required parameters supported.
 *   ID = the CAN ID to send
 *   len = the length of the expected response
 *
 *        1        2  3
 *     "rtrsend 0x123 5"    - send Standard Remote CAN frame with ID 0x123 and with expected response length of 5 bytes
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_rtrsend(int8_t argc, char * argv[])
{
    int32_t val;
    int8_t paramtype;
    uint16_t id_val;
    int8_t len;

    if (argc < 3)
    {
        return CMDLINE_TOO_FEW_ARGS;
    }
    else if (argc > 3)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else
    {
        // get the ID to send
        paramtype = CmdLine.ParseParam(argv[ARG1], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 0) || ((uint32_t)val > CAN_SFF_MASK))
        {
            return CMDLINE_INVALID_ARG;
        }
        id_val = val;

        // get the length to send
        paramtype = CmdLine.ParseParam(argv[ARG2], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 0) || (val > CAN_MAX_MESSAGE_LENGTH))
        {
            return CMDLINE_INVALID_ARG;
        }
        len = val;
    }

    if (CheckSendingMode() == MCP2515_FAIL)
    {
        return MCP2515_FAIL;
    }

    if (CheckSendingResponse(Mcp2515.MCP2515_RtrSend(id_val, len)) != MCP2515_OK)
    {
        return MCP2515_FAIL;
    }

    // Return success.
    return 0;
}

/*
 * NAME:
 *  int8_t Cmd_xrtrsnd(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "xrtrsnd" command to send Extended Remote CAN frame.
 *
 *  Two required parameters supported.
 *   ID = the CAN ID to send
 *   len = the length of the expected response
 *
 *        1        2  3
 *     "xrtrsnd 0x123 5"    - send Extended Remote CAN frame with ID 0x123 and with expected response length of 5 bytes
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_xrtrsnd(int8_t argc, char * argv[])
{
    int32_t val;
    int8_t paramtype;
    uint32_t id_val;
    int8_t len;

    if (argc < 3)
    {
        return CMDLINE_TOO_FEW_ARGS;
    }
    else if (argc > 3)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else
    {
        // get the ID to send
        paramtype = CmdLine.ParseParam(argv[ARG1], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 0) || ((uint32_t)val > CAN_EFF_MASK))
        {
            return CMDLINE_INVALID_ARG;
        }
        id_val = val;

        // get the length to send
        paramtype = CmdLine.ParseParam(argv[ARG2], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 0) || (val > CAN_MAX_MESSAGE_LENGTH))
        {
            return CMDLINE_INVALID_ARG;
        }
        len = val;
    }

    if (CheckSendingMode() == MCP2515_FAIL)
    {
        return MCP2515_FAIL;
    }

    if (CheckSendingResponse(Mcp2515.MCP2515_ExtRtrSend(id_val, len)) != MCP2515_OK)
    {
        return MCP2515_FAIL;
    }

    // Return success.
    return 0;
}

/*
 * NAME:
 *  int8_t Cmd_recv(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "recv" command to show received CAN message.
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_recv(int8_t argc __attribute__((unused)), char * argv[] __attribute__((unused)))
{
    CAN_FRAME frame;

    if (Mcp2515.MCP2515_Recv(&frame) == MCP2515_OK)
    {
        ShowCAN_Msg(&frame);

        if (Mcp2515.MCP2515_Recv(&frame) == MCP2515_OK)
        {
            ShowCAN_Msg(&frame);

            if (Mcp2515.MCP2515_Recv(&frame) == MCP2515_OK)
            {
                ShowCAN_Msg(&frame);
            }
        }
    }
    else
    {
        Serial.println(F("No CAN Message Received!"));
    }

    // Return success.
    return 0;
}

// show CAN message
void ShowCAN_Msg(CAN_FRAME * frame)
{
    uint32_t id = frame->can_id;

    // cnt tstamp RxB  type  ID      DLC  ------------------- message --------------------
    // 45:158193:RXB0::ID: 0x00E  DLC: 8  CAN Message: 61 73 64 66 61 73 64 66  'asdfasdf'  // 'norm'
    // -or-
    // 45,158193,0x00E,8,61,73,64,66,61,73,64,66                                            // 'csv'
    // -or-
    // 45:158193  0x00E  8  61 73 64 66 61 73 64 66                                         // 'min'

    Serial.print(CAN_MsgCnt);

    switch (CAN_MsgFormat)
    {
        case CMSGS_CSV:
            Serial.print(',');
            break;
        case CMSGS_MIN:
        case CMSGS_NORM:
            Serial.print(':');
            break;
    }
    Serial.print(millis());

    switch (CAN_MsgFormat)
    {
        case CMSGS_NORM:
            Serial.print(F(":RXB"));
            Serial.print(frame->can_rxb);
            Serial.print(F("::"));
            break;
        case CMSGS_CSV:
            Serial.print(',');
            break;
        case CMSGS_MIN:
            Serial.print(F("  "));
            break;
    }

    if (frame->can_id & CAN_EFF_FLAG)   // extended ID
    {
        id &= CAN_EFF_MASK;
        if (CAN_MsgFormat == CMSGS_NORM)
        {
            Serial.print(F("EID: "));
        }
        Serial.print(F("0x"));
        Serial.print((id >> 24) & 0xff);
        PrintHexByte((id >> 16) & 0xff);
        Serial.print('_');
        PrintHexByte((id >> 8) & 0xff);
    }
    else    // standard ID
    {
        id &= CAN_SFF_MASK;
        if (CAN_MsgFormat == CMSGS_NORM)
        {
            Serial.print(F("ID: "));
        }
        Serial.print(F("0x"));
        Serial.print((id >> 8) & 0x07);
    }
    PrintHexByte(id & 0xff);

    switch (CAN_MsgFormat)
    {
        case CMSGS_CSV:
            Serial.print(',');
            break;
        case CMSGS_NORM:
            Serial.print(F("  DLC: "));
            break;
        case CMSGS_MIN:
            Serial.print(F("  "));
            break;
    }

    Serial.print(frame->can_dlc);

    switch (CAN_MsgFormat)
    {
        case CMSGS_CSV:
            Serial.print(',');
            break;
        case CMSGS_NORM:
        case CMSGS_MIN:
            Serial.print(F("  "));
            break;
    }

    if (frame->can_id & CAN_RTR_FLAG)       //  was RTR request
    {
        switch (CAN_MsgFormat)
        {
            case CMSGS_NORM:
                Serial.println(F("REMOTE REQUEST FRAME"));
                break;
            case CMSGS_CSV:
            case CMSGS_MIN:
                Serial.println(F("RTR"));
                break;
        }
        // DLK_TODO: Generate RTR response
    }
    else
    {
        if (CAN_MsgFormat == CMSGS_NORM)
        {
            Serial.print(F("CAN Message: "));
        }
        for (uint8_t i = 0; i < frame->can_dlc; ++i)
        {
            if (i != 0)
            {
                if (CAN_MsgFormat == CMSGS_CSV)
                {
                    Serial.print(',');
                }
                else
                {
                    Serial.print(' ');
                }
            }
            PrintHexByte(frame->can_data[i]);
        }
        if (CAN_MsgFormat == CMSGS_NORM)
        {
            Serial.print(F(" '"));
            for (uint8_t i = 0; i < frame->can_dlc; ++i)
            {
                if (isprint(frame->can_data[i]))
                {
                    Serial.print((char)frame->can_data[i]);
                }
                else
                {
                    Serial.print('.');
                }
            }
            Serial.print('\'');
        }
        Serial.println();
    }
}

/*
 * NAME:
 *  int8_t Cmd_stat(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "stat" command to show MCP2515 Status.
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_stat(int8_t argc __attribute__((unused)), char * argv[] __attribute__((unused)))
{
    ShowStatus();

    // Return success.
    return 0;
}

// show MCP2515 Status
void ShowStatus(void)
{
    uint8_t status;

    status = Mcp2515.MCP2515_ReadStatus();

    Serial.print(F("Status: "));
    Print0xHexByte(status);
    Serial.println();

    Serial.print(F(" - [7]TX2IF:  "));
    Serial.print(F("TXB2 Empty Interrupt =  "));
    if (status & STAT_TX2IF)
    {
    }
    else
    {
        Serial.print(F("No "));
    }
    Serial.println(F("Interrupt is pending"));

    Serial.print(F(" - [6]TX2REQ: "));
    Serial.print(F("TXB2 Transmit Request = "));
    if (status & STAT_TX2REQ)
    {
    }
    else
    {
        Serial.print(F("Not "));
    }
    Serial.println(F("Pending transmission"));

    Serial.print(F(" - [5]TX1IF:  "));
    Serial.print(F("TXB1 Empty Interrupt  = "));
    if (status & STAT_TX1IF)
    {
    }
    else
    {
        Serial.print(F("No "));
    }
    Serial.println(F("Interrupt is pending"));

    Serial.print(F(" - [4]TX1REQ: "));
    Serial.print(F("TXB1 Transmit Request = "));
    if (status & STAT_TX1REQ)
    {
    }
    else
    {
        Serial.print(F("Not "));
    }
    Serial.println(F("Pending transmission"));

    Serial.print(F(" - [3]TX0IF:  "));
    Serial.print(F("TXB0 Empty Interrupt  = "));
    if (status & STAT_TX0IF)
    {
    }
    else
    {
        Serial.print(F("No "));
    }
    Serial.println(F("Interrupt is pending"));

    Serial.print(F(" - [2]TX0REQ: "));
    Serial.print(F("TXB0 Transmit Request = "));
    if (status & STAT_TX0REQ)
    {
    }
    else
    {
        Serial.print(F("Not "));
    }
    Serial.println(F("Pending transmission"));

    Serial.print(F(" - [1]RX1IF:  "));
    Serial.print(F("RXB1 Full Interrupt   = "));
    if (status & STAT_RX1IF)
    {
    }
    else
    {
        Serial.print(F("No "));
    }
    Serial.println(F("Interrupt is pending"));

    Serial.print(F(" - [0]RX0IF:  "));
    Serial.print(F("RXB0 Full Interrupt   = "));
    if (status & STAT_RX0IF)
    {
    }
    else
    {
        Serial.print(F("No "));
    }
    Serial.println(F("Interrupt is pending"));
}

/*
 * NAME:
 *  int8_t Cmd_rxstat(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "rxstat" command to show MCP2515 Rx Status.
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_rxstat(int8_t argc __attribute__((unused)), char * argv[] __attribute__((unused)))
{
    ShowRX_Status();

    // Return success.
    return 0;
}

// show MCP2515 Rx Status
void ShowRX_Status(void)
{
    uint8_t status;

    status = Mcp2515.MCP2515_ReadRxStatus();

    Serial.print(F("Rx Status: "));
    Print0xHexByte(status);
    Serial.println();

    Serial.print(F(" - [7:6]RXM[1:0]:  Receive message = "));
    switch (status & RXMS_MASK)
    {
        case RXM_NO_MSG:
            Serial.println(F("No Rx message"));
            break;
        case RXM_RXB0_MSG:
            Serial.println(F("Message in RXB0"));
            break;
        case RXM_RXB1_MSG:
            Serial.println(F("Message in RXB1"));
            break;
        case RXM_RXBOTH_MSG:
            Serial.println(F("Messages in RXB0 and RXB1"));
            break;
    }

    Serial.print(F(" - [4:3]MTYP[1:0]: Message type    = "));
    switch (status & MTYP_MASK)
    {
        case MTYP_STD_DATA:
            Serial.println(F("Standard data frame"));
            break;
        case MTYP_STD_REM:
            Serial.println(F("Standard remote frame"));
            break;
        case MTYP_EXT_DATA:
            Serial.println(F("Extended data frame"));
            break;
        case MTYP_EXT_REM:
            Serial.println(F("Extended remote frame"));
            break;
    }

    Serial.print(F(" - [2:0]FM[2:0]:   Filter match    = RXF"));
    switch (status & FM_MASK)
    {
        case FM_RXF0:
            Serial.println('0');
            break;
        case FM_RXF1:
            Serial.println('1');
            break;
        case FM_RXF2:
            Serial.println('2');
            break;
        case FM_RXF3:
            Serial.println('3');
            break;
        case FM_RXF4:
            Serial.println('4');
            break;
        case FM_RXF5:
            Serial.println('5');
            break;
        case FM_RXF0_RXB1:
            Serial.println(F("0 rollover to RXB1"));
            break;
        case FM_RXF1_RXB1:
            Serial.println(F("1 rollover to RXB1"));
            break;
    }
}

/*
 * NAME:
 *  int8_t Cmd_init(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "init" command to reset and initialize MCP2515.
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_init(int8_t argc __attribute__((unused)), char * argv[] __attribute__((unused)))
{
    return DoCAN_Init(CAN_Speed);
}

// do MCP2515 CAN initialization
uint8_t DoCAN_Init(uint8_t canSpeed)
{
    if (Mcp2515.MCP2515_Init(canSpeed) != MCP2515_OK)
    {
        return MCP2515_FAIL;
    }

    // set CAN mode
    Mcp2515.MCP2515_SetMode(CAN_Mode);
    Serial.print("Mode: ");
    ShowCAN_Mode(CAN_Mode);

    // init CAN masks and CAN filters
    for (uint8_t i = 0; i < MCP2515_N_MASKS; ++i)
    {
        if (CAN_Masks[i] & CAN_EFF_FLAG)      // extended
        {
            Mcp2515.MCP2515_SetExtMask(i, CAN_Masks[i] & CAN_EFF_MASK);
        }
        else
        {
            Mcp2515.MCP2515_SetMask(i, CAN_Masks[i] >> 16, CAN_Masks[i], CAN_Masks[i] >> 8);
        }
    }

    for (uint8_t i = 0; i < MCP2515_N_FILTERS; ++i)
    {
        if (CAN_Filters[i] & CAN_EFF_FLAG)    // extended
        {
            Mcp2515.MCP2515_SetExtFilter(i, CAN_Filters[i] & CAN_EFF_MASK);
        }
        else
        {
            Mcp2515.MCP2515_SetFilter(i, CAN_Filters[i] >> 16, CAN_Filters[i], CAN_Filters[i] >> 8);
        }
    }
    return MCP2515_OK;
}

/*
 * NAME:
 *  int8_t Cmd_inte(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "inte" command to show MCP2515 interrupts enabled in CANINTE register.
 *
 *  One optional parameter supported.
 *   bits = sets/clears CANINTE interrupts enabled/disabled
 *
 *        1   2
 *     "inte"       - show MCP2515 CANINTE register
 *     "inte 0xa5"  - set 0xa5 in the MCP2515 CANINTE register
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_inte(int8_t argc, char * argv[])
{
    int32_t val;
    int8_t paramtype;
    uint8_t regval;

    if (argc < 2)
    {
        // OK, just show MCP2515 CANINTE register
    }
    else if (argc > 2)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else
    {
        // get the register value
        paramtype = CmdLine.ParseParam(argv[ARG1], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 0) || (val > 0xff))
        {
            return CMDLINE_INVALID_ARG;
        }
        regval = val;
        Mcp2515.MCP2515_WriteRegister(MCP2515_CANINTE, regval);
    }

    ShowCANINTE_Reg();      // show MCP2515 CANINTE register

    // Return success.
    return 0;
}

// show MCP2515 CANINTE register
void ShowCANINTE_Reg(void)
{
    uint8_t reg;

    reg = Mcp2515.MCP2515_ReadRegister(MCP2515_CANINTE);

    Serial.print(F("CANINTE("));
    Print0xHexByte(MCP2515_CANINTE);
    Serial.print(F("): "));
    Print0xHexByte(reg);
    Serial.println();

    Serial.print(F(" - [7]MERRE: "));
    Serial.print(F("Message Error Interrupt           = "));
    if (reg & MCP2515_MERRF)
    {
        Serial.print(F("En"));
    }
    else
    {
        Serial.print(F("Dis"));
    }
    Serial.println(F("abled"));

    Serial.print(F(" - [6]WAKIE: "));
    Serial.print(F("Wake-up Interrupt                 = "));
    if (reg & MCP2515_WAKIF)
    {
        Serial.print(F("En"));
    }
    else
    {
        Serial.print(F("Dis"));
    }
    Serial.println(F("abled"));

    Serial.print(F(" - [5]ERRIE: "));
    Serial.print(F("Error Interrupt                   = "));
    if (reg & MCP2515_ERRIF)
    {
        Serial.print(F("En"));
    }
    else
    {
        Serial.print(F("Dis"));
    }
    Serial.println(F("abled"));

    Serial.print(F(" - [4]TX2IE: "));
    Serial.print(F("Transmit Buffer 2 Empty Interrupt = "));
    if (reg & MCP2515_TX2IF)
    {
        Serial.print(F("En"));
    }
    else
    {
        Serial.print(F("Dis"));
    }
    Serial.println(F("abled"));

    Serial.print(F(" - [3]TX1IE: "));
    Serial.print(F("Transmit Buffer 1 Empty Interrupt = "));
    if (reg & MCP2515_TX1IF)
    {
        Serial.print(F("En"));
    }
    else
    {
        Serial.print(F("Dis"));
    }
    Serial.println(F("abled"));

    Serial.print(F(" - [2]TX0IE: "));
    Serial.print(F("Transmit Buffer 0 Empty Interrupt = "));
    if (reg & MCP2515_TX0IF)
    {
        Serial.print(F("En"));
    }
    else
    {
        Serial.print(F("Dis"));
    }
    Serial.println(F("abled"));

    Serial.print(F(" - [1]RX1IE: "));
    Serial.print(F("Receive Buffer 1 Full Interrupt   = "));
    if (reg & MCP2515_RX1IF)
    {
        Serial.print(F("En"));
    }
    else
    {
        Serial.print(F("Dis"));
    }
    Serial.println(F("abled"));

    Serial.print(F(" - [0]RX0IE: "));
    Serial.print(F("Receive Buffer 0 Full Interrupt   = "));
    if (reg & MCP2515_RX0IF)
    {
        Serial.print(F("En"));
    }
    else
    {
        Serial.print(F("Dis"));
    }
    Serial.println(F("abled"));
}

/*
 * NAME:
 *  int8_t Cmd_ints(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "ints" command to show/set/clear MCP2515 interrupts in CANINTF register.
 *
 *  Two optional parameters supported.
 *   <clr> = clears the CANINTF register
 *    -or-
 *   bits = sets/clears CANINTF interrupts
 *
 *        1   2
 *     "ints"       - show MCP2515 CANINTE register
 *     "ints 0xa5"  - set 0xa5 in the MCP2515 CANINTF register
 *     "ints clr"   - clear interrupts in the MCP2515 CANINTF register
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  Note: Clearing all interrupts may not actually clear all interrupts if any
 *        interrupt is pending (i.e. if the receive buffers are full and the MAB contains
 *        CAN data, clearing the RXnIF bits clears the full buffers but one of the bits
 *        may be set again when the MAB is now allowed to be transferred to an Rx buffer
 *        after which an RXnIF bit will be set).
 */
int8_t Cmd_ints(int8_t argc, char * argv[])
{
    int32_t val;
    int8_t paramtype;
    uint8_t regval;

    if (argc < 2)
    {
        // OK, just show MCP2515 CANINTF register
    }
    else if (argc > 2)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else
    {
        // get the register value
        paramtype = CmdLine.ParseParam(argv[ARG1], &val);
        if (paramtype == BADPARAM)      // probably text input
        {
            if (strcmp_P(argv[ARG1], PSTR("clr")) == 0)
            {
                Mcp2515.MCP2515_WriteRegister(MCP2515_CANINTF, 0);
            }
            else
            {
                return CMDLINE_INVALID_ARG;
            }
        }
        else if ((paramtype == STRVAL) || (val < 0) || (val > 0xff))
        {
            return CMDLINE_INVALID_ARG;
        }
        else
        {
            regval = val;
            Mcp2515.MCP2515_WriteRegister(MCP2515_CANINTF, regval);
        }
    }

    ShowCANINTF_Reg();      // show MCP2515 CANINTF register

    // Return success.
    return 0;
}

// show MCP2515 CANINTF register
void ShowCANINTF_Reg(void)
{
    uint8_t reg;

    reg = Mcp2515.MCP2515_ReadRegister(MCP2515_CANINTF);

    Serial.print(F("CANINTF("));
    Print0xHexByte(MCP2515_CANINTF);
    Serial.print(F("): "));
    Print0xHexByte(reg);
    Serial.println();

    Serial.print(F(" - [7]MERRF: "));
    Serial.print(F("Message Error Interrupt           = "));
    if (reg & MCP2515_MERRF)
    {
    }
    else
    {
        Serial.print(F("Not "));
    }
    Serial.println(F("Pending"));

    Serial.print(F(" - [6]WAKIF: "));
    Serial.print(F("Wake-up Interrupt                 = "));
    if (reg & MCP2515_WAKIF)
    {
    }
    else
    {
        Serial.print(F("Not "));
    }
    Serial.println(F("Pending"));

    Serial.print(F(" - [5]ERRIF: "));
    Serial.print(F("Error Interrupt                   = "));
    if (reg & MCP2515_ERRIF)
    {
    }
    else
    {
        Serial.print(F("Not "));
    }
    Serial.println(F("Pending"));

    Serial.print(F(" - [4]TX2IF: "));
    Serial.print(F("Transmit Buffer 2 Empty Interrupt = "));
    if (reg & MCP2515_TX2IF)
    {
    }
    else
    {
        Serial.print(F("Not "));
    }
    Serial.println(F("Pending"));

    Serial.print(F(" - [3]TX1IF: "));
    Serial.print(F("Transmit Buffer 1 Empty Interrupt = "));
    if (reg & MCP2515_TX1IF)
    {
    }
    else
    {
        Serial.print(F("Not "));
    }
    Serial.println(F("Pending"));

    Serial.print(F(" - [2]TX0IF: "));
    Serial.print(F("Transmit Buffer 0 Empty Interrupt = "));
    if (reg & MCP2515_TX0IF)
    {
    }
    else
    {
        Serial.print(F("Not "));
    }
    Serial.println(F("Pending"));

    Serial.print(F(" - [1]RX1IF: "));
    Serial.print(F("Receive Buffer 1 Full Interrupt   = "));
    if (reg & MCP2515_RX1IF)
    {
    }
    else
    {
        Serial.print(F("Not "));
    }
    Serial.println(F("Pending"));

    Serial.print(F(" - [0]RX0IF: "));
    Serial.print(F("Receive Buffer 0 Full Interrupt   = "));
    if (reg & MCP2515_RX0IF)
    {
    }
    else
    {
        Serial.print(F("Not "));
    }
    Serial.println(F("Pending"));
}

/*
 * NAME:
 *  int8_t Cmd_errs(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "errs" command to show MCP2515 errors in EFLG register and the
 *  TEC and REC error counter registers.
 *
 *  One optional parameter supported.
 *   <clr> = clears the RX1OVR and RX0OVR bits in the EFLG register
 *
 *        1   2
 *     "errs"       - show MCP2515 EFLG register
 *     "errs clr"   - clear errors in the MCP2515 EFLG register
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_errs(int8_t argc, char * argv[])
{
    if (argc < 2)
    {
        // OK, just show MCP2515 EFLG register
    }
    else if (argc > 2)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else
    {
        if (strcmp_P(argv[ARG1], PSTR("clr")) == 0)
        {
            // clear RX1OVR and RX0OVR overflow error bits
            Mcp2515.MCP2515_WriteRegister(MCP2515_EFLG, 0);
        }
        else
        {
            return CMDLINE_INVALID_ARG;
        }
    }

    ShowErrors_Regs();      // show MCP2515 EFLG register (and TEC and REC registers)

    // Return success.
    return 0;
}

// show MCP2515 EFLG and TEC and REC registers
void ShowErrors_Regs(void)
{
    uint8_t reg;

    reg = Mcp2515.MCP2515_ReadRegister(MCP2515_EFLG);

    Serial.print(F("EFLG("));
    Print0xHexByte(MCP2515_EFLG);
    Serial.print(F("): "));
    Print0xHexByte(reg);
    Serial.println();

    Serial.print(F(" - [7]RX1OVR: "));
    Serial.print(F("Receive Buffer 1 Overflow = "));
    if (reg & MCP2515_EFLG_RX1OVR)
    {
        Serial.println(F("RX1 Overflow"));
    }
    else
    {
        Serial.println(F("OK"));
    }

    Serial.print(F(" - [6]RX0OVR: "));
    Serial.print(F("Receive Buffer 0 Overflow = "));
    if (reg & MCP2515_EFLG_RX0OVR)
    {
        Serial.println(F("RX0 Overflow"));
    }
    else
    {
        Serial.println(F("OK"));
    }

    Serial.print(F(" - [5]TXBO:   "));
    Serial.print(F("Bus-Off Error             = "));
    if (reg & MCP2515_EFLG_TXBO)
    {
        Serial.println(F("TEC == 255"));
    }
    else
    {
        Serial.println(F("OK"));
    }

    Serial.print(F(" - [4]TXEP:   "));
    Serial.print(F("Transmit Error-Passive    = "));
    if (reg & MCP2515_EFLG_TXEP)
    {
        Serial.println(F("TEC >= 128"));
    }
    else
    {
        Serial.println(F("OK"));
    }

    Serial.print(F(" - [3]RXEP:   "));
    Serial.print(F("Receive Error-Passive     = "));
    if (reg & MCP2515_EFLG_RXEP)
    {
        Serial.println(F("REC >= 128"));
    }
    else
    {
        Serial.println(F("OK"));
    }

    Serial.print(F(" - [2]TXWAR:  "));
    Serial.print(F("Transmit Error Warning    = "));
    if (reg & MCP2515_EFLG_TXWAR)
    {
        Serial.println(F("TEC >= 96"));
    }
    else
    {
        Serial.println(F("OK"));
    }

    Serial.print(F(" - [1]RXWAR:  "));
    Serial.print(F("Receive Error Warning     = "));
    if (reg & MCP2515_EFLG_RXWAR)
    {
        Serial.println(F("REC >= 96"));
    }
    else
    {
        Serial.println(F("OK"));
    }

    Serial.print(F(" - [0]EWARN:  "));
    Serial.print(F("Error Warning Flag        = "));
    if (reg & MCP2515_EFLG_EWARN)
    {
        Serial.println(F("TEC or REC >= 96"));
    }
    else
    {
        Serial.println(F("OK"));
    }

    reg = Mcp2515.MCP2515_ReadRegister(MCP2515_TEC);
    Serial.print(F("TEC("));
    Print0xHexByte(MCP2515_TEC);
    Serial.print(F("): "));
    Serial.println(reg);

    reg = Mcp2515.MCP2515_ReadRegister(MCP2515_REC);
    Serial.print(F("REC("));
    Print0xHexByte(MCP2515_REC);
    Serial.print(F("): "));
    Serial.println(reg);
}

/*
 * NAME:
 *  int8_t Cmd_filt(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "filt" command to show/set MCP2515 Standard Filter registers.
 *
 *  Four optional parameters supported.
 *   n = specific filter to show/set (default is show all filters)
 *   ID = filter ID to set
 *   D0 = filter Data0 to set
 *   D1 = filter Data1 to set
 *
 *        1  2   3   4 5
 *     "filt"               - show all MCP2515 Standard Filter registers
 *     "filt 0"             - show Filter1 MCP2515 Standard Filter registers
 *     "filt 5"             - show Filter5 MCP2515 Standard Filter registers
 *     "filt 2 0x123"       - set Standard Filter2 for CAN ID 0x123
 *     "filt 3 0x123 6 7"   - set Standard Filter3 for CAN ID 0x123 and Data0 to 6 and Data1 to 7
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  Only supports standard 11-bit CAN data frames
 *  Setting the CAN bus standard filter is saved in EEPROM.
 */
int8_t Cmd_filt(int8_t argc, char * argv[])
{
    int32_t val;
    int8_t paramtype;
    uint8_t filt_num = 99;
    uint16_t filt_id = 0;
    uint8_t filt_d0 = 0;
    uint8_t filt_d1 = 0;

    if (argc > 5)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else if (argc > 1)
    {
        // get the filter number
        paramtype = CmdLine.ParseParam(argv[ARG1], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 0) || (val > 5))
        {
            return CMDLINE_INVALID_ARG;
        }
        filt_num = val;

        if (argc > 2)       // set Filter registers
        {
            // get the filter ID (11-bit only)
            paramtype = CmdLine.ParseParam(argv[ARG2], &val);
            if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
                (val < 0) || ((uint32_t)val > CAN_SFF_MASK))
            {
                return CMDLINE_INVALID_ARG;
            }
            filt_id = val;

            if (argc > 3)
            {
                // get the filter Data0
                paramtype = CmdLine.ParseParam(argv[ARG3], &val);
                if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
                    (val < 0) || (val > 0xff))
                {
                    return CMDLINE_INVALID_ARG;
                }
                filt_d0 = val;

                if (argc > 4)
                {
                    // get the filter Data1
                    paramtype = CmdLine.ParseParam(argv[ARG4], &val);
                    if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
                        (val < 0) || (val > 0xff))
                    {
                        return CMDLINE_INVALID_ARG;
                    }
                    filt_d1 = val;
                }
            }
            Mcp2515.MCP2515_SetFilter(filt_num, filt_id, filt_d0, filt_d1);
            val = (uint32_t)filt_id << 16;
            val |= (((uint32_t)filt_d1 << 8) | filt_d0);
            CAN_Filters[filt_num] = val;
            EEPROM.put(EE_CAN_FILTERS + (filt_num * sizeof(uint32_t)), val);
            CommitEEPROM();
        }
    }

    ShowFilters_Regs(filt_num, false);    // show MCP2515 Filter registers

    // Return success.
    return 0;
}

/*
 * NAME:
 *  int8_t Cmd_xfilt(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "xfilt" command to show/set MCP2515 Extended Filter registers.
 *
 *  Two optional parameters supported.
 *   n = specific filter to show/set (default is show all filters)
 *   ID = filter ID to set
 *
 *        1   2   3
 *     "xfilt"               - show all MCP2515 Extended Filter registers
 *     "xfilt 0"             - show Filter1 MCP2515 Extended Filter registers
 *     "xfilt 5"             - show Filter5 MCP2515 Extended Filter registers
 *     "xfilt 2 0x123"       - set Extended Filter2 for CAN ID 0x123
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  Only supports extended 29-bit CAN data frames
 *  Setting the CAN bus extended filter is saved in EEPROM.
 */
int8_t Cmd_xfilt(int8_t argc, char * argv[])
{
    int32_t val;
    int8_t paramtype;
    uint8_t filt_num = 99;

    if (argc > 3)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else if (argc > 1)
    {
        // get the filter number
        paramtype = CmdLine.ParseParam(argv[ARG1], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 0) || (val > 5))
        {
            return CMDLINE_INVALID_ARG;
        }
        filt_num = val;

        if (argc > 2)         // set Filter registers
        {
            // get the filter ID (11-bit only)
            paramtype = CmdLine.ParseParam(argv[ARG2], &val);
            if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
                (val < 0) || ((uint32_t)val > CAN_EFF_MASK))
            {
                return CMDLINE_INVALID_ARG;
            }
            Mcp2515.MCP2515_SetExtFilter(filt_num, val);
            val |= CAN_EFF_FLAG;            // extended
            CAN_Filters[filt_num] = val;
            EEPROM.put(EE_CAN_FILTERS + (filt_num * sizeof(uint32_t)), val);
            CommitEEPROM();
        }
    }

    ShowFilters_Regs(filt_num, true);    // show MCP2515 Filter registers

    // Return success.
    return 0;
}

// show MCP2515 Filter registers
void ShowFilters_Regs(uint8_t filt_num, bool ext)
{
    const uint8_t filtregs[MCP2515_N_FILTERS] =
    {
        MCP2515_RXF0SIDH, MCP2515_RXF1SIDH, MCP2515_RXF2SIDH,
        MCP2515_RXF3SIDH, MCP2515_RXF4SIDH, MCP2515_RXF5SIDH
    };
    uint8_t regs[4];
    uint8_t filt_cnt = 6;
    uint8_t mode;
    uint32_t id;

    if (filt_num < filt_cnt)
    {
        filt_cnt = filt_num + 1;
    }
    else
    {
        filt_num = 0;
    }

    mode = Mcp2515.MCP2515_ReadRegister(MCP2515_CANCTRL) & MODE_MASK;   // get mode
    Mcp2515.MCP2515_SetMode(MODE_CONFIG);       // must be in Configuration mode to read Filter registers

    for (uint8_t i = filt_num; i < filt_cnt; ++i)
    {
        Serial.print(F("Filter"));
        Serial.print(i);
        Serial.print('_');
        Serial.print((i < 2) ? "0" : "1");
        Serial.print('(');

        // get filter regs
        Print0xHexByte(filtregs[i]);
        Mcp2515.MCP2515_ReadRegisters(filtregs[i], regs, 4);  // RXFnSIDH, RXFnSIDL, RXFnEID8, RXFnEID0

        Serial.print(F("):"));
        for (uint8_t j = 0; j < 4; ++j)
        {
            Serial.print(' ');
            Print0xHexByte(regs[j]);
        }

        id = (regs[0] << 3) + (regs[1] >> 5);

        Serial.print(F("  ("));
        if (ext)
        {
            if (regs[1] & MCP2515_RXB_IDE)
            {
                Serial.print(F("E"));
            }
            Serial.print(F("ID: 0x"));
            id = (id << 18);
            id |= (regs[3]);
            id |= ((uint32_t)regs[2] << 8);
            id |= ((uint32_t)(regs[1] & 0x03) << 16);
            PrintHexByte((id >> 24) & 0xff);
            PrintHexByte((id >> 16) & 0xff);
            Serial.print('_');
            PrintHexByte((id >> 8) & 0xff);
            PrintHexByte(id & 0xff);
        }
        else
        {
            if (regs[1] & MCP2515_RXB_IDE)
            {
                Serial.print(F("E"));
            }
            Serial.print(F("ID: 0x"));
            Serial.print((id & 0x700) >> 8, HEX);
            PrintHexByte(id & 0xff);

            Serial.print(F("  D0: "));
            Print0xHexByte(regs[2]);

            Serial.print(F("  D1: "));
            Print0xHexByte(regs[3]);
        }
        Serial.println(')');
    }

    Mcp2515.MCP2515_SetMode(mode);          // restore mode
}

/*
 * NAME:
 *  int8_t Cmd_mask(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "mask" command to show/set MCP2515 Mask registers.
 *
 *  Four optional parameters supported.
 *   n = specific mask to show/set (default is show all masks)
 *   ID = mask ID to set
 *   D0 = mask Data0 to set
 *   D1 = mask Data1 to set
 *
 *        1  2   3   4 5
 *     "mask"               - show all MCP2515 Standard Mask registers
 *     "mask 0"             - show Mask0 MCP2515 Standard Mask registers
 *     "mask 1"             - show Mask1 MCP2515 Standard Mask registers
 *     "mask 1 0x123"       - set Standard Mask1 for CAN ID 0x123
 *     "mask 0 0x123 6 7"   - set Standard Mask0 for CAN ID 0x123 and Data0 to 6 and Data1 to 7
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  Only supports standard 11-bit CAN data frames
 *  Setting the CAN bus standard mask is saved in EEPROM.
 */
int8_t Cmd_mask(int8_t argc, char * argv[])
{
    int32_t val;
    int8_t paramtype;
    uint8_t mask_num = 99;
    uint16_t mask_id = 0;
    uint8_t mask_d0 = MASK_ANY_DATA;
    uint8_t mask_d1 = MASK_ANY_DATA;

    if (argc > 5)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else if (argc > 1)
    {
        // get the mask number
        paramtype = CmdLine.ParseParam(argv[ARG1], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 0) || (val > 1))
        {
            return CMDLINE_INVALID_ARG;
        }
        mask_num = val;

        if (argc > 2)         // set Mask registers
        {
            // get the mask ID (11-bit only)
            paramtype = CmdLine.ParseParam(argv[ARG2], &val);
            if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
                (val < 0) || ((uint32_t)val > CAN_SFF_MASK))
            {
                return CMDLINE_INVALID_ARG;
            }
            mask_id = val;

            if (argc > 3)
            {
                // get the mask Data0
                paramtype = CmdLine.ParseParam(argv[ARG3], &val);
                if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
                    (val < 0) || (val > 0xff))
                {
                    return CMDLINE_INVALID_ARG;
                }
                mask_d0 = val;

                if (argc > 4)
                {
                    // get the mask Data1
                    paramtype = CmdLine.ParseParam(argv[ARG4], &val);
                    if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
                        (val < 0) || (val > 0xff))
                    {
                        return CMDLINE_INVALID_ARG;
                    }
                    mask_d1 = val;
                }
            }
            Mcp2515.MCP2515_SetMask(mask_num, mask_id, mask_d0, mask_d1);
            val = (uint32_t)mask_id << 16;
            val |= (((uint32_t)mask_d1 << 8) | mask_d0);
            CAN_Masks[mask_num] = val;
            EEPROM.put(EE_CAN_MASKS + (mask_num * sizeof(uint32_t)), val);
            CommitEEPROM();
        }
    }

    ShowMasks_Regs(mask_num, false);    // show MCP2515 Mask registers

    // Return success.
    return 0;
}

/*
 * NAME:
 *  int8_t Cmd_xmask(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "xmask" command to show/set MCP2515 Extended Mask registers.
 *
 *  Two optional parameters supported.
 *   n = specific mask to show/set (default is show all masks)
 *   ID = mask ID to set
 *
 *        1   2   3
 *     "xmask"               - show all MCP2515 Extended Mask registers
 *     "xmask 0"             - show Mask0 MCP2515 Extended Mask registers
 *     "xmask 1"             - show Mask1 MCP2515 Extended Mask registers
 *     "xmask 0 0x1234"      - set Extended Mask2 for CAN ID 0x1234
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  Only supports standard 11-bit CAN data frames
 *  Setting the CAN bus extended mask is saved in EEPROM.
 */
int8_t Cmd_xmask(int8_t argc, char * argv[])
{
    int32_t val;
    int8_t paramtype;
    uint8_t mask_num = 99;

    if (argc > 3)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else if (argc > 1)
    {
        // get the mask number
        paramtype = CmdLine.ParseParam(argv[ARG1], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 0) || (val > 1))
        {
            return CMDLINE_INVALID_ARG;
        }
        mask_num = val;

        if (argc > 2)       // set Mask registers
        {
            // get the mask ID (11-bit only)
            paramtype = CmdLine.ParseParam(argv[ARG2], &val);
            if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
                (val < 0) || ((uint32_t)val > CAN_EFF_MASK))
            {
                return CMDLINE_INVALID_ARG;
            }
            Mcp2515.MCP2515_SetExtMask(mask_num, val);
            val |= CAN_EFF_FLAG;            // extended
            CAN_Masks[mask_num] = val;
            EEPROM.put(EE_CAN_MASKS + (mask_num * sizeof(uint32_t)), val);
            CommitEEPROM();
        }
    }

    ShowMasks_Regs(mask_num, true);    // show MCP2515 Mask registers

    // Return success.
    return 0;
}

// show MCP2515 Mask registers
void ShowMasks_Regs(uint8_t mask_num, bool ext)
{
    const uint8_t maskregs[MCP2515_N_FILTERS] = { MCP2515_RXM0SIDH, MCP2515_RXM1SIDH };
    uint8_t regs[4];
    uint8_t mask_cnt = 2;
    uint8_t mode;
    uint32_t id;

    if (mask_num < mask_cnt)
    {
        mask_cnt = mask_num + 1;
    }
    else
    {
        mask_num = 0;
    }

    mode = Mcp2515.MCP2515_ReadRegister(MCP2515_CANCTRL) & MODE_MASK;   // get mode
    Mcp2515.MCP2515_SetMode(MODE_CONFIG);       // must be in Configuration mode to read Mask registers

    for (uint8_t i = mask_num; i < mask_cnt; ++i)
    {
        Serial.print(F("Mask"));
        Serial.print(i);
        Serial.print('(');

        // get mask regs
        Print0xHexByte(maskregs[i]);
        Mcp2515.MCP2515_ReadRegisters(maskregs[i], regs, 4);  // RXMnSIDH, RXMnSIDL, RXMnEID8, RXMnEID0

        Serial.print(F("):"));
        for (uint8_t j = 0; j < 4; ++j)
        {
            Serial.print(' ');
            Print0xHexByte(regs[j]);
        }

        id = (regs[0] << 3) + (regs[1] >> 5);

        if (ext)
        {
            id = (id << 18);
            id |= (regs[3]);
            id |= ((uint32_t)regs[2] << 8);
            id |= ((uint32_t)(regs[1] & 0x03) << 16);
            Serial.print(F("  (EID: "));
            if (id == 0x00000000)
            {
                Serial.print(F("Any"));
            }
            else
            {
                Print0xHexByte((id >> 24) & 0xff);
                PrintHexByte((id >> 16) & 0xff);
                Serial.print('_');
                PrintHexByte((id >> 8) & 0xff);
                PrintHexByte(id & 0xff);
            }
        }
        else
        {
            id &= CAN_SFF_MASK;
            Serial.print(F(" (ID: "));
            if (id == 0x000)
            {
                Serial.print(F("Any  "));
            }
            else
            {
                Serial.print(F("0x"));
                Serial.print(id >> 8, HEX);
                PrintHexByte(id & 0xff);
            }

            Serial.print(F("  D0: "));
            if (regs[2] == 0x00)
            {
                Serial.print(F("Any "));
            }
            else
            {
                Print0xHexByte(regs[2]);
            }

            Serial.print(F("  D1: "));
            if (regs[3] == 0x00)
            {
                Serial.print(F("Any"));
            }
            else
            {
                Print0xHexByte(regs[3]);
            }
        }
        Serial.println(')');
    }

    Mcp2515.MCP2515_SetMode(mode);          // restore mode

//                           |                         |                       |
// 31 30 29  28 27  26 25 24  23 22 21 20 19 18 17 16  15 14 13 12 11  10 9 8  7 6 5 4 3 2 1 0
//           10  9   8  7  6   5  4  3  2  1  0 17 16  15 14 13 12 11 10  9 8  7 6 5 4 3 2 1 0
//           ------------[0]---------- --[1]--- --[1]- ---------[2]----------  ------[3]------
//
//
}

/*
 * NAME:
 *  int8_t Cmd_rxmode(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "rxmode" command to show/set MCP2515 Rx operating mode(s).
 *
 *  Two optional parameters supported.
 *   n = specific Rx buffer show/set (default is show all Rx buffers)
 *   <all> = mode to set to receive all CAN messages
 *      -or-
 *   <filt> = mode to set to receive only valid filtered CAN messages
 *
 *        1    2  3
 *     "rxmode"             - show MCP2515 RXB0 and RXB1 operating modes
 *     "rxmode 0 all"       - set MCP2515 RXB0 operating mode to receive all CAN messages
 *     "rxmode 1 filt"      - set MCP2515 RXB1 operating mode to receive valid filtered CAN messages
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  Only supports standard 11-bit CAN data frames
 */
int8_t Cmd_rxmode(int8_t argc, char * argv[])
{
    int32_t val;
    int8_t paramtype;
    uint8_t rxb_num = 99;

    if (argc > 3)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else if (argc > 1)
    {
        // get the Rx buffer number
        paramtype = CmdLine.ParseParam(argv[ARG1], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL) ||
            (val < 0) || (val > 1))
        {
            return CMDLINE_INVALID_ARG;
        }
        rxb_num = val;

        if (argc > 2)
        {
            if (strcmp_P(argv[ARG2], PSTR("all")) == 0)
            {
                // set RXBnCTRL Rx operating mode bits to receive all CAN messages
                Mcp2515.MCP2515_SetRxMode(rxb_num, RXM_MASK);
            }
            else if (strcmp_P(argv[ARG2], PSTR("filt")) == 0)
            {
                // set RXBnCTRL Rx operating mode bits to receive only valid filtered CAN messages
                Mcp2515.MCP2515_SetRxMode(rxb_num, 0);
            }
            else
            {
                return CMDLINE_INVALID_ARG;
            }
        }
    }

    ShowRxMode(rxb_num);    // show MCP2515 Rx operating mode(s)

    // Return success.
    return 0;
}

// show MCP2515 Rx operating mode(s)
void ShowRxMode(uint8_t rxb_num)
{
    uint8_t reg;

    switch (rxb_num)
    {
        case RXB0:
            reg = Mcp2515.MCP2515_ReadRegister(MCP2515_RXB0CTRL);
            ShowRxModeInfo(rxb_num, reg);
            break;
        case RXB1:
            reg = Mcp2515.MCP2515_ReadRegister(MCP2515_RXB1CTRL);
            ShowRxModeInfo(rxb_num, reg);
            break;
        default:
            reg = Mcp2515.MCP2515_ReadRegister(MCP2515_RXB0CTRL);
            ShowRxModeInfo(RXB0, reg);

            reg = Mcp2515.MCP2515_ReadRegister(MCP2515_RXB1CTRL);
            ShowRxModeInfo(RXB1, reg);
            break;
    }
}

// show MCP2515 Rx operating mode info
void ShowRxModeInfo(uint8_t rxb_num, uint8_t reg)
{
    Serial.print(F("RXB"));
    Serial.print(rxb_num);
    Serial.print(F(" Rx Operating mode = "));
    Serial.print(F("Receives "));
    switch (reg & RXM_MASK)
    {
        case RXM_M3:
            Serial.println(F("any message                 <all>"));
            break;
        case RXM_M0:
            Serial.println(F("all valid filtered messages <filt>"));
            break;
    }
}

/*
 * NAME:
 *  int8_t Cmd_cnf(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "cnf" command to show MCP2515 CNFn registers and set the CAN bus speed.
 *
 *  One optional parameter supported.
 *   Kspd = sets the CAN bus speed in Kbps
 *
 *       1   2
 *     "cnf"        - show MCP2515 CNFn registers the CAN bus speed
 *     "cnf 250"    - set MCP2515 CNFn registers for 250 Kbs CAN bus speed
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  Setting the CAN bus speed is saved in EEPROM.
 */
int8_t Cmd_cnf(int8_t argc, char * argv[])
{
    int32_t val;
    int8_t paramtype;

    if (argc > 2)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else if (argc > 1)
    {
        // get the register value
        paramtype = CmdLine.ParseParam(argv[ARG1], &val);
        if ((paramtype == BADPARAM) || (paramtype == STRVAL))
        {
            return CMDLINE_INVALID_ARG;
        }

        switch (val)
        {
            case 5:
                CAN_Speed = CAN_5KBPS;
                break;
            case 10:
                CAN_Speed = CAN_10KBPS;
                break;
            case 20:
                CAN_Speed = CAN_20KBPS;
                break;
            case 31:
                CAN_Speed = CAN_31K25BPS;
                break;
            case 33:
                CAN_Speed = CAN_33KBPS;
                break;
            case 40:
                CAN_Speed = CAN_40KBPS;
                break;
            case 50:
                CAN_Speed = CAN_50KBPS;
                break;
            case 80:
                CAN_Speed = CAN_80KBPS;
                break;
            case 83:
                CAN_Speed = CAN_83K3BPS;
                break;
            case 95:
                CAN_Speed = CAN_95KBPS;
                break;
            case 100:
                CAN_Speed = CAN_100KBPS;
                break;
            case 125:
                CAN_Speed = CAN_125KBPS;
                break;
            case 200:
                CAN_Speed = CAN_200KBPS;
                break;
            case 250:
                CAN_Speed = CAN_250KBPS;
                break;
            case 500:
                CAN_Speed = CAN_500KBPS;
                break;
            case 1000:
                CAN_Speed = CAN_1000KBPS;
                break;
            default:
                return CMDLINE_INVALID_ARG;
                break;
        }
        Mcp2515.MCP2515_SetBitrate(CAN_Speed);

#if defined(ESP8266) || defined(ESP32)
        EEPROM.write(EE_CAN_SPEED, CAN_Speed);
#else
        EEPROM.update(EE_CAN_SPEED, CAN_Speed);
#endif
        CommitEEPROM();
    }
    ShowCNF_Regs(false);     // show MCP2515 CNFn registers

    // Return success.
    return 0;
}

/* 250 Kbs @ 8 Mhz
CNF1(0x2A): 0x00
 - [7:6]SJW[1:0]: Synchronization Jump Width Length = 1 x Tq = 250 nS
 - [5:0]BRP[5:0]: Baud Rate Prescaler = Tq = 2 / Fosc = 250 nS

CNF2(0x29): 0xB1
 - [7]BTLMODE:       PS2 Bit Time Length = determined by the CNF3::PHSEG2[2:0]
 - [6]SAM:           Sample Point Configuration = Bus line sampled once at the sample point
 - [5:3]PHSEG1[2:0]: PS1 Length = 7 x Tq = 1750 nS
 - [2:0]PRSEG[2:0]:  Propagation Segment Length = 2 x Tq = 500 nS

CNF3(0x28): 0x85
 - [7]SOF:           Start-of-Frame signal = CLKOUT pin is SOF signal
 - [6]WAKFIL:        Wake-up filter = disabled
 - [2:0]PHSEG2[2:0]: PS2 Length = 6 x Tq = 1500 nS

Can Speed = 250 Kbps
*/
/* 500 Kbs @ 8 Mhz
CNF1(0x2A): 0x00
 - [7:6]SJW[1:0]: Synchronization Jump Width Length = 1 x Tq = 250 nS
 - [5:0]BRP[5:0]: Baud Rate Prescaler = Tq = 2 / Fosc = 250 nS

CNF2(0x29): 0x90
 - [7]BTLMODE:       PS2 Bit Time Length = determined by the CNF3::PHSEG2[2:0]
 - [6]SAM:           Sample Point Configuration = Bus line sampled once at the sample point
 - [5:3]PHSEG1[2:0]: PS1 Length = 3 x Tq = 750 nS
 - [2:0]PRSEG[2:0]:  Propagation Segment Length = 1 x Tq = 250 nS

CNF3(0x28): 0x82
 - [7]SOF:           Start-of-Frame signal = CLKOUT pin is SOF signal
 - [6]WAKFIL:        Wake-up filter = disabled
 - [2:0]PHSEG2[2:0]: PS2 Length = 3 x Tq = 750 nS

Can Speed = 500 Kbps
*/
/* 125 Kbs @ 8 Mhz
CNF1(0x2A): 0x01
 - [7:6]SJW[1:0]: Synchronization Jump Width Length = 1 x Tq = 500 nS
 - [5:0]BRP[5:0]: Baud Rate Prescaler = Tq = 4 / Fosc = 500 nS

CNF2(0x29): 0xB1
 - [7]BTLMODE:       PS2 Bit Time Length = determined by the CNF3::PHSEG2[2:0]
 - [6]SAM:           Sample Point Configuration = Bus line sampled once at the sample point
 - [5:3]PHSEG1[2:0]: PS1 Length = 7 x Tq = 3500 nS
 - [2:0]PRSEG[2:0]:  Propagation Segment Length = 2 x Tq = 1000 nS

CNF3(0x28): 0x85
 - [7]SOF:           Start-of-Frame signal = CLKOUT pin is SOF signal
 - [6]WAKFIL:        Wake-up filter = disabled
 - [2:0]PHSEG2[2:0]: PS2 Length = 6 x Tq = 3000 nS

Can Speed = 125 Kbps
*/
// show MCP2515 CNFn registers
void ShowCNF_Regs(bool speed_only)
{
    uint16_t tosc = 1000 / 8;       // nS (1 / 8 Mhz)
    uint16_t Tq;                    // nS
    uint32_t num_tqs = 0;
    uint8_t reg;
    uint8_t cnf1;
    uint8_t cnf2;
    uint8_t cnf3;

    reg = Mcp2515.MCP2515_ReadRegister(MCP2515_CANCTRL);

    cnf1 = Mcp2515.MCP2515_ReadRegister(MCP2515_CNF1);
    cnf2 = Mcp2515.MCP2515_ReadRegister(MCP2515_CNF2);
    cnf3 = Mcp2515.MCP2515_ReadRegister(MCP2515_CNF3);

    if (!speed_only)
    {
        Serial.print(F("CNF1("));
        Print0xHexByte(MCP2515_CNF1);
        Serial.print(F("): "));
        Print0xHexByte(cnf1);
        Serial.println();
    }

    Tq = (2 * ((cnf1 & BRP_BITS) + 1) * tosc);

    if (!speed_only)
    {
        Serial.print(F(" - [7:6]SJW[1:0]: "));
        Serial.print(F("Synchronization Jump Width Length = "));
        switch (cnf1 & SJW_MASK)
        {
            case SJW4:
                Serial.print('4');
                break;
            case SJW3:
                Serial.print('3');
                break;
            case SJW2:
                Serial.print('2');
                break;
            case SJW1:
                Serial.print('1');
                break;
        }
        Serial.print(F(" x Tq = "));
        Serial.print((((cnf1 & SJW_MASK) >> 6) + 1) * Tq);
        Serial.println(F(" nS"));
    }
    num_tqs += (((cnf1 & SJW_MASK) >> 6) + 1);

    if (!speed_only)
    {
        Serial.print(F(" - [5:0]BRP[5:0]: "));
        Serial.print(F("Baud Rate Prescaler = "));
        Serial.print(F("Tq = "));
        Serial.print(2 * ((cnf1 & BRP_BITS) + 1));
        Serial.print(F(" / Fosc = "));
        Serial.print(Tq);
        Serial.println(F(" nS"));


        Serial.print(F("CNF2("));
        Print0xHexByte(MCP2515_CNF2);
        Serial.print(F("): "));
        Print0xHexByte(cnf2);
        Serial.println();

        Serial.print(F(" - [7]BTLMODE:       "));
        Serial.print(F("PS2 Bit Time Length = "));
        if (cnf2 & BTLMODE)
        {
            Serial.println(F("determined by the CNF3::PHSEG2[2:0]"));
        }
        else
        {
            Serial.println(F("greater of PS1 and IPT (2 x Tq)"));
        }
        Serial.print(F(" - [6]SAM:           "));
        Serial.print(F("Sample Point Config = "));
        Serial.print(F("Bus line sampled "));
        if (cnf2 & SAMPLE_3X)
        {
            Serial.print(F("three times"));
        }
        else
        {
            Serial.print(F("once"));
        }
        Serial.println(F(" at sample point"));

        Serial.print(F(" - [5:3]PHSEG1[2:0]: "));
        Serial.print(F("PS1 Length = "));
        Serial.print(((cnf2 & PHSEG1_BITS) >> 3) + 1);
        Serial.print(F(" x Tq = "));
        Serial.print((((cnf2 & PHSEG1_BITS) >> 3) + 1) * Tq);
        Serial.println(F(" nS"));
    }
    num_tqs += (((cnf2 & PHSEG1_BITS) >> 3) + 1);

    if (!speed_only)
    {
        Serial.print(F(" - [2:0]PRSEG[2:0]:  "));
        Serial.print(F("Propagation Segment Length = "));
        Serial.print((cnf2 & PRSEG_BITS) + 1);
        Serial.print(F(" x Tq = "));
        Serial.print(((cnf2 & PRSEG_BITS) + 1) * Tq);
        Serial.println(F(" nS"));
    }
    num_tqs += ((cnf2 & PRSEG_BITS) + 1);


    if (!speed_only)
    {
        Serial.print(F("CNF3("));
        Print0xHexByte(MCP2515_CNF3);
        Serial.print(F("): "));
        Print0xHexByte(cnf3);
        Serial.println();

        Serial.print(F(" - [7]SOF:           "));
        Serial.print(F("Start-of-Frame signal = "));
        if (reg & CLKOUT_ENABLE)
        {
            Serial.print(F("CLKOUT pin is "));
            if (cnf3 & SOF_ENABLE)
            {
                Serial.println(F("SOF signal"));
            }
            else
            {
                Serial.println(F("clock out"));
            }
        }
        else
        {
            Serial.println(F("don't care"));
        }

        Serial.print(F(" - [6]WAKFIL:        "));
        Serial.print(F("Wake-up filter = "));
        if (cnf3 & WAKFIL_ENABLE)
        {
            Serial.print(F("en"));
        }
        else
        {
            Serial.print(F("dis"));
        }
        Serial.println(F("abled"));

        Serial.print(F(" - [2:0]PHSEG2[2:0]: "));
        Serial.print(F("PS2 Length = "));
        Serial.print((cnf3 & PHSEG2_BITS) + 1);
        Serial.print(F(" x Tq = "));
        Serial.print(((cnf3 & PHSEG2_BITS) + 1) * Tq);
        Serial.println(F(" nS"));
    }
    num_tqs += ((cnf3 & PHSEG2_BITS) + 1);

    Serial.print(F("Can Speed = "));
    Serial.print(1000000L / (num_tqs * Tq));
    Serial.println(F(" Kbps"));
}

/*
 * NAME:
 *  int8_t Cmd_cmsg(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "cmsg" command to show/set reported CAN message format.
 *
 *  One optional parameter supported.
 *   <min> = set to show minimal received CAN messages
 *      -or-
 *   <csv> = set to show received CAN messages as comma-separated values (CSV)
 *      -or-
 *   <norm> = set to show full received CAN messages
 *
 *        1   2
 *     "cmsg"       - show reported CAN message format
 *     "cmsg min"   - set reported CAN message format to minimal
 *     "cmsg csv"   - set reported CAN message format to CSV (comma-separated-values)
 *     "cmsg norm"  - set reported CAN message format to normal expanded messages
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  Setting the CAN message format is saved in EEPROM.
 */
int8_t Cmd_cmsg(int8_t argc, char * argv[])
{
    if (argc > 2)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else if (argc > 1)
    {
        if (strcmp_P(argv[ARG1], PSTR("min")) == 0)
        {
            CAN_MsgFormat = CMSGS_MIN;
        }
        else if (strcmp_P(argv[ARG1], PSTR("csv")) == 0)
        {
            CAN_MsgFormat = CMSGS_CSV;
        }
        else if (strcmp_P(argv[ARG1], PSTR("norm")) == 0)
        {
            CAN_MsgFormat = CMSGS_NORM;
        }
        else
        {
            return CMDLINE_INVALID_ARG;
        }

#if defined(ESP8266) || defined(ESP32)
        EEPROM.write(EE_CAN_MSGS_FORMAT, CAN_MsgFormat);
#else
        EEPROM.update(EE_CAN_MSGS_FORMAT, CAN_MsgFormat);
#endif
        CommitEEPROM();
    }

    ShowCAN_MsgFormat(CAN_MsgFormat);

    // Return success.
    return 0;
}

void ShowCAN_MsgFormat(uint8_t msg_format)
{
    Serial.print(F("CAN Msgs: "));
    switch (msg_format)
    {
        case CMSGS_MIN:
            Serial.println(F("min"));
            break;
        case CMSGS_CSV:
            Serial.println(F("csv"));
            break;
        case CMSGS_NORM:
            Serial.println(F("norm"));
            break;
    }
}

/*
 * NAME:
 *  int8_t Cmd_dupe(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "dupe" command to show/set allow showing duplicate CAN messages.
 *
 *  One optional parameter supported.
 *   <y> = set to allow showing duplicate CAN messages
 *      -or-
 *   <n> = set to not allow showing duplicate CAN messages
 *
 *        1  2
 *     "dupe"       - show allow showing duplicate CAN messages setting
 *     "dupe y"     - allow showing duplicate CAN messages
 *     "dupe n"     - don't allow showing duplicate CAN messages
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  Setting the allow showing duplicate CAN messages is saved in EEPROM.
 */
int8_t Cmd_dupe(int8_t argc, char * argv[])
{
     char dir_val;

    if (argc > 2)
    {
        return CMDLINE_TOO_MANY_ARGS;
    }
    else if (argc > 1)
    {
        // get the allow showing duplicate CAN messages value
        dir_val = tolower(argv[ARG1][0]);
        if ((dir_val != 'y') && (dir_val != 'n'))
        {
            return CMDLINE_INVALID_ARG;
        }
        if (dir_val == 'y')
        {
            CAN_MsgDupes = CDUPE_YES;
        }
        else if (dir_val == 'n')
        {
            CAN_MsgDupes = CDUPE_NO;
        }
        else
        {
            return CMDLINE_INVALID_ARG;
        }

#if defined(ESP8266) || defined(ESP32)
        EEPROM.write(EE_CAN_DUPE_MSGS, CAN_MsgDupes);
#else
        EEPROM.update(EE_CAN_DUPE_MSGS, CAN_MsgDupes);
#endif
        CommitEEPROM();
    }

    ShowDupeMsgAllowed(CAN_MsgDupes);

    // Return success.
    return 0;
}

void ShowDupeMsgAllowed(uint8_t dupe_allowed)
{
    Serial.print(F("Showing duplicate CAN messages: "));
    if (dupe_allowed == CDUPE_YES)
    {
        Serial.println(F("Yes"));
    }
    else
    {
        Serial.println(F("No"));
    }
}

/*
 * NAME:
 *  int8_t Cmd_cfg(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "cfg" command to show key configuration items.
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_cfg(int8_t argc __attribute__((unused)), char * argv[] __attribute__((unused)))
{
    uint8_t mode;

    mode = Mcp2515.MCP2515_ReadRegister(MCP2515_CANCTRL) & MODE_MASK;
    Serial.print(F("Mode: "));
    ShowCAN_Mode(mode);         // show MCP2515 mode

    ShowCNF_Regs(true);         // show MCP2515 CNFn speed
    
    ShowCAN_MsgFormat(CAN_MsgFormat);

    ShowDupeMsgAllowed(CAN_MsgDupes);

    // Return success.
    return 0;
}

/*
 * NAME:
 *  int8_t Cmd_help(int8_t argc, char * argv[])
 *
 * PARAMETERS:
 *  int8_t argc = number of command line arguments for the command
 *  char * argv[] = pointer to array of parameters associated with the command
 *
 * WHAT:
 *  Implements the "help" command to display a simple list of the available
 *  commands with a brief description of each command.
 *
 *  One optional parameter supported.
 *   <cls> = clears the output screen using ANSI escape sequence
 *
 * RETURN VALUES:
 *  int8_t = 0 = command successfully processed
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
int8_t Cmd_help(int8_t argc, char * argv[])
{
    if (argc > 1)
    {
        if (strcmp_P(argv[ARG1], PSTR("cls")) == 0)
        {
            Serial.println(F(CLS_HOME));
            return 0;
        }
    }

    // Print some header text.
    Serial.println();
    Serial.println(TITLE_MSG);
    Serial.println(F("Available commands"));
    Serial.println(F("------------------"));

    CmdLine.ShowCommands();     // show commands menu with help information

    // Return success.
    return 0;
}

/*
 * NAME:
 *  void PrintHexByte(uint8_t the_byte)
 *
 * PARAMETERS:
 *  uint8_t the_byte = the byte to print
 *
 * WHAT:
 *  Helper function that prints formatted hex byte value to console.
 *
 * RETURN VALUES:
 *  None.
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
void PrintHexByte(uint8_t the_byte)
{
    if (the_byte < 0x10)
    {
        Serial.print('0');          // leading '0'
    }
    Serial.print(the_byte, HEX);
}

/*
 * NAME:
 *  void Print0xHexByte(uint8_t the_byte)
 *
 * PARAMETERS:
 *  uint8_t the_byte = the byte to print
 *
 * WHAT:
 *  Helper function that prints formatted hex byte value with leading "0x" to console.
 *
 * RETURN VALUES:
 *  None.
 *
 * SPECIAL CONSIDERATIONS:
 *  None.
 */
void Print0xHexByte(uint8_t the_byte)
{
    Serial.print(F("0x"));          // leading "0x"
    PrintHexByte(the_byte);
}

void CommitEEPROM(void)
{
#if defined(ARDUINO_ARCH_RP2040) || defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
#endif
}

