#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <bitset>
#include <QRegularExpression>

namespace chess {

// ZOBRIST HASHING
// for zobrist hashing w/ and polyglot opening books
const quint64 POLYGLOT_RANDOM_64[781] = {
  Q_UINT64_C(0x9D39247E33776D41), Q_UINT64_C(0x2AF7398005AAA5C7), Q_UINT64_C(0x44DB015024623547),
  Q_UINT64_C(0x9C15F73E62A76AE2), Q_UINT64_C(0x75834465489C0C89), Q_UINT64_C(0x3290AC3A203001BF),
  Q_UINT64_C(0x0FBBAD1F61042279), Q_UINT64_C(0xE83A908FF2FB60CA), Q_UINT64_C(0x0D7E765D58755C10),
  Q_UINT64_C(0x1A083822CEAFE02D), Q_UINT64_C(0x9605D5F0E25EC3B0), Q_UINT64_C(0xD021FF5CD13A2ED5),
  Q_UINT64_C(0x40BDF15D4A672E32), Q_UINT64_C(0x011355146FD56395), Q_UINT64_C(0x5DB4832046F3D9E5),
  Q_UINT64_C(0x239F8B2D7FF719CC), Q_UINT64_C(0x05D1A1AE85B49AA1), Q_UINT64_C(0x679F848F6E8FC971),
  Q_UINT64_C(0x7449BBFF801FED0B), Q_UINT64_C(0x7D11CDB1C3B7ADF0), Q_UINT64_C(0x82C7709E781EB7CC),
  Q_UINT64_C(0xF3218F1C9510786C), Q_UINT64_C(0x331478F3AF51BBE6), Q_UINT64_C(0x4BB38DE5E7219443),
  Q_UINT64_C(0xAA649C6EBCFD50FC), Q_UINT64_C(0x8DBD98A352AFD40B), Q_UINT64_C(0x87D2074B81D79217),
  Q_UINT64_C(0x19F3C751D3E92AE1), Q_UINT64_C(0xB4AB30F062B19ABF), Q_UINT64_C(0x7B0500AC42047AC4),
  Q_UINT64_C(0xC9452CA81A09D85D), Q_UINT64_C(0x24AA6C514DA27500), Q_UINT64_C(0x4C9F34427501B447),
  Q_UINT64_C(0x14A68FD73C910841), Q_UINT64_C(0xA71B9B83461CBD93), Q_UINT64_C(0x03488B95B0F1850F),
  Q_UINT64_C(0x637B2B34FF93C040), Q_UINT64_C(0x09D1BC9A3DD90A94), Q_UINT64_C(0x3575668334A1DD3B),
  Q_UINT64_C(0x735E2B97A4C45A23), Q_UINT64_C(0x18727070F1BD400B), Q_UINT64_C(0x1FCBACD259BF02E7),
  Q_UINT64_C(0xD310A7C2CE9B6555), Q_UINT64_C(0xBF983FE0FE5D8244), Q_UINT64_C(0x9F74D14F7454A824),
  Q_UINT64_C(0x51EBDC4AB9BA3035), Q_UINT64_C(0x5C82C505DB9AB0FA), Q_UINT64_C(0xFCF7FE8A3430B241),
  Q_UINT64_C(0x3253A729B9BA3DDE), Q_UINT64_C(0x8C74C368081B3075), Q_UINT64_C(0xB9BC6C87167C33E7),
  Q_UINT64_C(0x7EF48F2B83024E20), Q_UINT64_C(0x11D505D4C351BD7F), Q_UINT64_C(0x6568FCA92C76A243),
  Q_UINT64_C(0x4DE0B0F40F32A7B8), Q_UINT64_C(0x96D693460CC37E5D), Q_UINT64_C(0x42E240CB63689F2F),
  Q_UINT64_C(0x6D2BDCDAE2919661), Q_UINT64_C(0x42880B0236E4D951), Q_UINT64_C(0x5F0F4A5898171BB6),
  Q_UINT64_C(0x39F890F579F92F88), Q_UINT64_C(0x93C5B5F47356388B), Q_UINT64_C(0x63DC359D8D231B78),
  Q_UINT64_C(0xEC16CA8AEA98AD76), Q_UINT64_C(0x5355F900C2A82DC7), Q_UINT64_C(0x07FB9F855A997142),
  Q_UINT64_C(0x5093417AA8A7ED5E), Q_UINT64_C(0x7BCBC38DA25A7F3C), Q_UINT64_C(0x19FC8A768CF4B6D4),
  Q_UINT64_C(0x637A7780DECFC0D9), Q_UINT64_C(0x8249A47AEE0E41F7), Q_UINT64_C(0x79AD695501E7D1E8),
  Q_UINT64_C(0x14ACBAF4777D5776), Q_UINT64_C(0xF145B6BECCDEA195), Q_UINT64_C(0xDABF2AC8201752FC),
  Q_UINT64_C(0x24C3C94DF9C8D3F6), Q_UINT64_C(0xBB6E2924F03912EA), Q_UINT64_C(0x0CE26C0B95C980D9),
  Q_UINT64_C(0xA49CD132BFBF7CC4), Q_UINT64_C(0xE99D662AF4243939), Q_UINT64_C(0x27E6AD7891165C3F),
  Q_UINT64_C(0x8535F040B9744FF1), Q_UINT64_C(0x54B3F4FA5F40D873), Q_UINT64_C(0x72B12C32127FED2B),
  Q_UINT64_C(0xEE954D3C7B411F47), Q_UINT64_C(0x9A85AC909A24EAA1), Q_UINT64_C(0x70AC4CD9F04F21F5),
  Q_UINT64_C(0xF9B89D3E99A075C2), Q_UINT64_C(0x87B3E2B2B5C907B1), Q_UINT64_C(0xA366E5B8C54F48B8),
  Q_UINT64_C(0xAE4A9346CC3F7CF2), Q_UINT64_C(0x1920C04D47267BBD), Q_UINT64_C(0x87BF02C6B49E2AE9),
  Q_UINT64_C(0x092237AC237F3859), Q_UINT64_C(0xFF07F64EF8ED14D0), Q_UINT64_C(0x8DE8DCA9F03CC54E),
  Q_UINT64_C(0x9C1633264DB49C89), Q_UINT64_C(0xB3F22C3D0B0B38ED), Q_UINT64_C(0x390E5FB44D01144B),
  Q_UINT64_C(0x5BFEA5B4712768E9), Q_UINT64_C(0x1E1032911FA78984), Q_UINT64_C(0x9A74ACB964E78CB3),
  Q_UINT64_C(0x4F80F7A035DAFB04), Q_UINT64_C(0x6304D09A0B3738C4), Q_UINT64_C(0x2171E64683023A08),
  Q_UINT64_C(0x5B9B63EB9CEFF80C), Q_UINT64_C(0x506AACF489889342), Q_UINT64_C(0x1881AFC9A3A701D6),
  Q_UINT64_C(0x6503080440750644), Q_UINT64_C(0xDFD395339CDBF4A7), Q_UINT64_C(0xEF927DBCF00C20F2),
  Q_UINT64_C(0x7B32F7D1E03680EC), Q_UINT64_C(0xB9FD7620E7316243), Q_UINT64_C(0x05A7E8A57DB91B77),
  Q_UINT64_C(0xB5889C6E15630A75), Q_UINT64_C(0x4A750A09CE9573F7), Q_UINT64_C(0xCF464CEC899A2F8A),
  Q_UINT64_C(0xF538639CE705B824), Q_UINT64_C(0x3C79A0FF5580EF7F), Q_UINT64_C(0xEDE6C87F8477609D),
  Q_UINT64_C(0x799E81F05BC93F31), Q_UINT64_C(0x86536B8CF3428A8C), Q_UINT64_C(0x97D7374C60087B73),
  Q_UINT64_C(0xA246637CFF328532), Q_UINT64_C(0x043FCAE60CC0EBA0), Q_UINT64_C(0x920E449535DD359E),
  Q_UINT64_C(0x70EB093B15B290CC), Q_UINT64_C(0x73A1921916591CBD), Q_UINT64_C(0x56436C9FE1A1AA8D),
  Q_UINT64_C(0xEFAC4B70633B8F81), Q_UINT64_C(0xBB215798D45DF7AF), Q_UINT64_C(0x45F20042F24F1768),
  Q_UINT64_C(0x930F80F4E8EB7462), Q_UINT64_C(0xFF6712FFCFD75EA1), Q_UINT64_C(0xAE623FD67468AA70),
  Q_UINT64_C(0xDD2C5BC84BC8D8FC), Q_UINT64_C(0x7EED120D54CF2DD9), Q_UINT64_C(0x22FE545401165F1C),
  Q_UINT64_C(0xC91800E98FB99929), Q_UINT64_C(0x808BD68E6AC10365), Q_UINT64_C(0xDEC468145B7605F6),
  Q_UINT64_C(0x1BEDE3A3AEF53302), Q_UINT64_C(0x43539603D6C55602), Q_UINT64_C(0xAA969B5C691CCB7A),
  Q_UINT64_C(0xA87832D392EFEE56), Q_UINT64_C(0x65942C7B3C7E11AE), Q_UINT64_C(0xDED2D633CAD004F6),
  Q_UINT64_C(0x21F08570F420E565), Q_UINT64_C(0xB415938D7DA94E3C), Q_UINT64_C(0x91B859E59ECB6350),
  Q_UINT64_C(0x10CFF333E0ED804A), Q_UINT64_C(0x28AED140BE0BB7DD), Q_UINT64_C(0xC5CC1D89724FA456),
  Q_UINT64_C(0x5648F680F11A2741), Q_UINT64_C(0x2D255069F0B7DAB3), Q_UINT64_C(0x9BC5A38EF729ABD4),
  Q_UINT64_C(0xEF2F054308F6A2BC), Q_UINT64_C(0xAF2042F5CC5C2858), Q_UINT64_C(0x480412BAB7F5BE2A),
  Q_UINT64_C(0xAEF3AF4A563DFE43), Q_UINT64_C(0x19AFE59AE451497F), Q_UINT64_C(0x52593803DFF1E840),
  Q_UINT64_C(0xF4F076E65F2CE6F0), Q_UINT64_C(0x11379625747D5AF3), Q_UINT64_C(0xBCE5D2248682C115),
  Q_UINT64_C(0x9DA4243DE836994F), Q_UINT64_C(0x066F70B33FE09017), Q_UINT64_C(0x4DC4DE189B671A1C),
  Q_UINT64_C(0x51039AB7712457C3), Q_UINT64_C(0xC07A3F80C31FB4B4), Q_UINT64_C(0xB46EE9C5E64A6E7C),
  Q_UINT64_C(0xB3819A42ABE61C87), Q_UINT64_C(0x21A007933A522A20), Q_UINT64_C(0x2DF16F761598AA4F),
  Q_UINT64_C(0x763C4A1371B368FD), Q_UINT64_C(0xF793C46702E086A0), Q_UINT64_C(0xD7288E012AEB8D31),
  Q_UINT64_C(0xDE336A2A4BC1C44B), Q_UINT64_C(0x0BF692B38D079F23), Q_UINT64_C(0x2C604A7A177326B3),
  Q_UINT64_C(0x4850E73E03EB6064), Q_UINT64_C(0xCFC447F1E53C8E1B), Q_UINT64_C(0xB05CA3F564268D99),
  Q_UINT64_C(0x9AE182C8BC9474E8), Q_UINT64_C(0xA4FC4BD4FC5558CA), Q_UINT64_C(0xE755178D58FC4E76),
  Q_UINT64_C(0x69B97DB1A4C03DFE), Q_UINT64_C(0xF9B5B7C4ACC67C96), Q_UINT64_C(0xFC6A82D64B8655FB),
  Q_UINT64_C(0x9C684CB6C4D24417), Q_UINT64_C(0x8EC97D2917456ED0), Q_UINT64_C(0x6703DF9D2924E97E),
  Q_UINT64_C(0xC547F57E42A7444E), Q_UINT64_C(0x78E37644E7CAD29E), Q_UINT64_C(0xFE9A44E9362F05FA),
  Q_UINT64_C(0x08BD35CC38336615), Q_UINT64_C(0x9315E5EB3A129ACE), Q_UINT64_C(0x94061B871E04DF75),
  Q_UINT64_C(0xDF1D9F9D784BA010), Q_UINT64_C(0x3BBA57B68871B59D), Q_UINT64_C(0xD2B7ADEEDED1F73F),
  Q_UINT64_C(0xF7A255D83BC373F8), Q_UINT64_C(0xD7F4F2448C0CEB81), Q_UINT64_C(0xD95BE88CD210FFA7),
  Q_UINT64_C(0x336F52F8FF4728E7), Q_UINT64_C(0xA74049DAC312AC71), Q_UINT64_C(0xA2F61BB6E437FDB5),
  Q_UINT64_C(0x4F2A5CB07F6A35B3), Q_UINT64_C(0x87D380BDA5BF7859), Q_UINT64_C(0x16B9F7E06C453A21),
  Q_UINT64_C(0x7BA2484C8A0FD54E), Q_UINT64_C(0xF3A678CAD9A2E38C), Q_UINT64_C(0x39B0BF7DDE437BA2),
  Q_UINT64_C(0xFCAF55C1BF8A4424), Q_UINT64_C(0x18FCF680573FA594), Q_UINT64_C(0x4C0563B89F495AC3),
  Q_UINT64_C(0x40E087931A00930D), Q_UINT64_C(0x8CFFA9412EB642C1), Q_UINT64_C(0x68CA39053261169F),
  Q_UINT64_C(0x7A1EE967D27579E2), Q_UINT64_C(0x9D1D60E5076F5B6F), Q_UINT64_C(0x3810E399B6F65BA2),
  Q_UINT64_C(0x32095B6D4AB5F9B1), Q_UINT64_C(0x35CAB62109DD038A), Q_UINT64_C(0xA90B24499FCFAFB1),
  Q_UINT64_C(0x77A225A07CC2C6BD), Q_UINT64_C(0x513E5E634C70E331), Q_UINT64_C(0x4361C0CA3F692F12),
  Q_UINT64_C(0xD941ACA44B20A45B), Q_UINT64_C(0x528F7C8602C5807B), Q_UINT64_C(0x52AB92BEB9613989),
  Q_UINT64_C(0x9D1DFA2EFC557F73), Q_UINT64_C(0x722FF175F572C348), Q_UINT64_C(0x1D1260A51107FE97),
  Q_UINT64_C(0x7A249A57EC0C9BA2), Q_UINT64_C(0x04208FE9E8F7F2D6), Q_UINT64_C(0x5A110C6058B920A0),
  Q_UINT64_C(0x0CD9A497658A5698), Q_UINT64_C(0x56FD23C8F9715A4C), Q_UINT64_C(0x284C847B9D887AAE),
  Q_UINT64_C(0x04FEABFBBDB619CB), Q_UINT64_C(0x742E1E651C60BA83), Q_UINT64_C(0x9A9632E65904AD3C),
  Q_UINT64_C(0x881B82A13B51B9E2), Q_UINT64_C(0x506E6744CD974924), Q_UINT64_C(0xB0183DB56FFC6A79),
  Q_UINT64_C(0x0ED9B915C66ED37E), Q_UINT64_C(0x5E11E86D5873D484), Q_UINT64_C(0xF678647E3519AC6E),
  Q_UINT64_C(0x1B85D488D0F20CC5), Q_UINT64_C(0xDAB9FE6525D89021), Q_UINT64_C(0x0D151D86ADB73615),
  Q_UINT64_C(0xA865A54EDCC0F019), Q_UINT64_C(0x93C42566AEF98FFB), Q_UINT64_C(0x99E7AFEABE000731),
  Q_UINT64_C(0x48CBFF086DDF285A), Q_UINT64_C(0x7F9B6AF1EBF78BAF), Q_UINT64_C(0x58627E1A149BBA21),
  Q_UINT64_C(0x2CD16E2ABD791E33), Q_UINT64_C(0xD363EFF5F0977996), Q_UINT64_C(0x0CE2A38C344A6EED),
  Q_UINT64_C(0x1A804AADB9CFA741), Q_UINT64_C(0x907F30421D78C5DE), Q_UINT64_C(0x501F65EDB3034D07),
  Q_UINT64_C(0x37624AE5A48FA6E9), Q_UINT64_C(0x957BAF61700CFF4E), Q_UINT64_C(0x3A6C27934E31188A),
  Q_UINT64_C(0xD49503536ABCA345), Q_UINT64_C(0x088E049589C432E0), Q_UINT64_C(0xF943AEE7FEBF21B8),
  Q_UINT64_C(0x6C3B8E3E336139D3), Q_UINT64_C(0x364F6FFA464EE52E), Q_UINT64_C(0xD60F6DCEDC314222),
  Q_UINT64_C(0x56963B0DCA418FC0), Q_UINT64_C(0x16F50EDF91E513AF), Q_UINT64_C(0xEF1955914B609F93),
  Q_UINT64_C(0x565601C0364E3228), Q_UINT64_C(0xECB53939887E8175), Q_UINT64_C(0xBAC7A9A18531294B),
  Q_UINT64_C(0xB344C470397BBA52), Q_UINT64_C(0x65D34954DAF3CEBD), Q_UINT64_C(0xB4B81B3FA97511E2),
  Q_UINT64_C(0xB422061193D6F6A7), Q_UINT64_C(0x071582401C38434D), Q_UINT64_C(0x7A13F18BBEDC4FF5),
  Q_UINT64_C(0xBC4097B116C524D2), Q_UINT64_C(0x59B97885E2F2EA28), Q_UINT64_C(0x99170A5DC3115544),
  Q_UINT64_C(0x6F423357E7C6A9F9), Q_UINT64_C(0x325928EE6E6F8794), Q_UINT64_C(0xD0E4366228B03343),
  Q_UINT64_C(0x565C31F7DE89EA27), Q_UINT64_C(0x30F5611484119414), Q_UINT64_C(0xD873DB391292ED4F),
  Q_UINT64_C(0x7BD94E1D8E17DEBC), Q_UINT64_C(0xC7D9F16864A76E94), Q_UINT64_C(0x947AE053EE56E63C),
  Q_UINT64_C(0xC8C93882F9475F5F), Q_UINT64_C(0x3A9BF55BA91F81CA), Q_UINT64_C(0xD9A11FBB3D9808E4),
  Q_UINT64_C(0x0FD22063EDC29FCA), Q_UINT64_C(0xB3F256D8ACA0B0B9), Q_UINT64_C(0xB03031A8B4516E84),
  Q_UINT64_C(0x35DD37D5871448AF), Q_UINT64_C(0xE9F6082B05542E4E), Q_UINT64_C(0xEBFAFA33D7254B59),
  Q_UINT64_C(0x9255ABB50D532280), Q_UINT64_C(0xB9AB4CE57F2D34F3), Q_UINT64_C(0x693501D628297551),
  Q_UINT64_C(0xC62C58F97DD949BF), Q_UINT64_C(0xCD454F8F19C5126A), Q_UINT64_C(0xBBE83F4ECC2BDECB),
  Q_UINT64_C(0xDC842B7E2819E230), Q_UINT64_C(0xBA89142E007503B8), Q_UINT64_C(0xA3BC941D0A5061CB),
  Q_UINT64_C(0xE9F6760E32CD8021), Q_UINT64_C(0x09C7E552BC76492F), Q_UINT64_C(0x852F54934DA55CC9),
  Q_UINT64_C(0x8107FCCF064FCF56), Q_UINT64_C(0x098954D51FFF6580), Q_UINT64_C(0x23B70EDB1955C4BF),
  Q_UINT64_C(0xC330DE426430F69D), Q_UINT64_C(0x4715ED43E8A45C0A), Q_UINT64_C(0xA8D7E4DAB780A08D),
  Q_UINT64_C(0x0572B974F03CE0BB), Q_UINT64_C(0xB57D2E985E1419C7), Q_UINT64_C(0xE8D9ECBE2CF3D73F),
  Q_UINT64_C(0x2FE4B17170E59750), Q_UINT64_C(0x11317BA87905E790), Q_UINT64_C(0x7FBF21EC8A1F45EC),
  Q_UINT64_C(0x1725CABFCB045B00), Q_UINT64_C(0x964E915CD5E2B207), Q_UINT64_C(0x3E2B8BCBF016D66D),
  Q_UINT64_C(0xBE7444E39328A0AC), Q_UINT64_C(0xF85B2B4FBCDE44B7), Q_UINT64_C(0x49353FEA39BA63B1),
  Q_UINT64_C(0x1DD01AAFCD53486A), Q_UINT64_C(0x1FCA8A92FD719F85), Q_UINT64_C(0xFC7C95D827357AFA),
  Q_UINT64_C(0x18A6A990C8B35EBD), Q_UINT64_C(0xCCCB7005C6B9C28D), Q_UINT64_C(0x3BDBB92C43B17F26),
  Q_UINT64_C(0xAA70B5B4F89695A2), Q_UINT64_C(0xE94C39A54A98307F), Q_UINT64_C(0xB7A0B174CFF6F36E),
  Q_UINT64_C(0xD4DBA84729AF48AD), Q_UINT64_C(0x2E18BC1AD9704A68), Q_UINT64_C(0x2DE0966DAF2F8B1C),
  Q_UINT64_C(0xB9C11D5B1E43A07E), Q_UINT64_C(0x64972D68DEE33360), Q_UINT64_C(0x94628D38D0C20584),
  Q_UINT64_C(0xDBC0D2B6AB90A559), Q_UINT64_C(0xD2733C4335C6A72F), Q_UINT64_C(0x7E75D99D94A70F4D),
  Q_UINT64_C(0x6CED1983376FA72B), Q_UINT64_C(0x97FCAACBF030BC24), Q_UINT64_C(0x7B77497B32503B12),
  Q_UINT64_C(0x8547EDDFB81CCB94), Q_UINT64_C(0x79999CDFF70902CB), Q_UINT64_C(0xCFFE1939438E9B24),
  Q_UINT64_C(0x829626E3892D95D7), Q_UINT64_C(0x92FAE24291F2B3F1), Q_UINT64_C(0x63E22C147B9C3403),
  Q_UINT64_C(0xC678B6D860284A1C), Q_UINT64_C(0x5873888850659AE7), Q_UINT64_C(0x0981DCD296A8736D),
  Q_UINT64_C(0x9F65789A6509A440), Q_UINT64_C(0x9FF38FED72E9052F), Q_UINT64_C(0xE479EE5B9930578C),
  Q_UINT64_C(0xE7F28ECD2D49EECD), Q_UINT64_C(0x56C074A581EA17FE), Q_UINT64_C(0x5544F7D774B14AEF),
  Q_UINT64_C(0x7B3F0195FC6F290F), Q_UINT64_C(0x12153635B2C0CF57), Q_UINT64_C(0x7F5126DBBA5E0CA7),
  Q_UINT64_C(0x7A76956C3EAFB413), Q_UINT64_C(0x3D5774A11D31AB39), Q_UINT64_C(0x8A1B083821F40CB4),
  Q_UINT64_C(0x7B4A38E32537DF62), Q_UINT64_C(0x950113646D1D6E03), Q_UINT64_C(0x4DA8979A0041E8A9),
  Q_UINT64_C(0x3BC36E078F7515D7), Q_UINT64_C(0x5D0A12F27AD310D1), Q_UINT64_C(0x7F9D1A2E1EBE1327),
  Q_UINT64_C(0xDA3A361B1C5157B1), Q_UINT64_C(0xDCDD7D20903D0C25), Q_UINT64_C(0x36833336D068F707),
  Q_UINT64_C(0xCE68341F79893389), Q_UINT64_C(0xAB9090168DD05F34), Q_UINT64_C(0x43954B3252DC25E5),
  Q_UINT64_C(0xB438C2B67F98E5E9), Q_UINT64_C(0x10DCD78E3851A492), Q_UINT64_C(0xDBC27AB5447822BF),
  Q_UINT64_C(0x9B3CDB65F82CA382), Q_UINT64_C(0xB67B7896167B4C84), Q_UINT64_C(0xBFCED1B0048EAC50),
  Q_UINT64_C(0xA9119B60369FFEBD), Q_UINT64_C(0x1FFF7AC80904BF45), Q_UINT64_C(0xAC12FB171817EEE7),
  Q_UINT64_C(0xAF08DA9177DDA93D), Q_UINT64_C(0x1B0CAB936E65C744), Q_UINT64_C(0xB559EB1D04E5E932),
  Q_UINT64_C(0xC37B45B3F8D6F2BA), Q_UINT64_C(0xC3A9DC228CAAC9E9), Q_UINT64_C(0xF3B8B6675A6507FF),
  Q_UINT64_C(0x9FC477DE4ED681DA), Q_UINT64_C(0x67378D8ECCEF96CB), Q_UINT64_C(0x6DD856D94D259236),
  Q_UINT64_C(0xA319CE15B0B4DB31), Q_UINT64_C(0x073973751F12DD5E), Q_UINT64_C(0x8A8E849EB32781A5),
  Q_UINT64_C(0xE1925C71285279F5), Q_UINT64_C(0x74C04BF1790C0EFE), Q_UINT64_C(0x4DDA48153C94938A),
  Q_UINT64_C(0x9D266D6A1CC0542C), Q_UINT64_C(0x7440FB816508C4FE), Q_UINT64_C(0x13328503DF48229F),
  Q_UINT64_C(0xD6BF7BAEE43CAC40), Q_UINT64_C(0x4838D65F6EF6748F), Q_UINT64_C(0x1E152328F3318DEA),
  Q_UINT64_C(0x8F8419A348F296BF), Q_UINT64_C(0x72C8834A5957B511), Q_UINT64_C(0xD7A023A73260B45C),
  Q_UINT64_C(0x94EBC8ABCFB56DAE), Q_UINT64_C(0x9FC10D0F989993E0), Q_UINT64_C(0xDE68A2355B93CAE6),
  Q_UINT64_C(0xA44CFE79AE538BBE), Q_UINT64_C(0x9D1D84FCCE371425), Q_UINT64_C(0x51D2B1AB2DDFB636),
  Q_UINT64_C(0x2FD7E4B9E72CD38C), Q_UINT64_C(0x65CA5B96B7552210), Q_UINT64_C(0xDD69A0D8AB3B546D),
  Q_UINT64_C(0x604D51B25FBF70E2), Q_UINT64_C(0x73AA8A564FB7AC9E), Q_UINT64_C(0x1A8C1E992B941148),
  Q_UINT64_C(0xAAC40A2703D9BEA0), Q_UINT64_C(0x764DBEAE7FA4F3A6), Q_UINT64_C(0x1E99B96E70A9BE8B),
  Q_UINT64_C(0x2C5E9DEB57EF4743), Q_UINT64_C(0x3A938FEE32D29981), Q_UINT64_C(0x26E6DB8FFDF5ADFE),
  Q_UINT64_C(0x469356C504EC9F9D), Q_UINT64_C(0xC8763C5B08D1908C), Q_UINT64_C(0x3F6C6AF859D80055),
  Q_UINT64_C(0x7F7CC39420A3A545), Q_UINT64_C(0x9BFB227EBDF4C5CE), Q_UINT64_C(0x89039D79D6FC5C5C),
  Q_UINT64_C(0x8FE88B57305E2AB6), Q_UINT64_C(0xA09E8C8C35AB96DE), Q_UINT64_C(0xFA7E393983325753),
  Q_UINT64_C(0xD6B6D0ECC617C699), Q_UINT64_C(0xDFEA21EA9E7557E3), Q_UINT64_C(0xB67C1FA481680AF8),
  Q_UINT64_C(0xCA1E3785A9E724E5), Q_UINT64_C(0x1CFC8BED0D681639), Q_UINT64_C(0xD18D8549D140CAEA),
  Q_UINT64_C(0x4ED0FE7E9DC91335), Q_UINT64_C(0xE4DBF0634473F5D2), Q_UINT64_C(0x1761F93A44D5AEFE),
  Q_UINT64_C(0x53898E4C3910DA55), Q_UINT64_C(0x734DE8181F6EC39A), Q_UINT64_C(0x2680B122BAA28D97),
  Q_UINT64_C(0x298AF231C85BAFAB), Q_UINT64_C(0x7983EED3740847D5), Q_UINT64_C(0x66C1A2A1A60CD889),
  Q_UINT64_C(0x9E17E49642A3E4C1), Q_UINT64_C(0xEDB454E7BADC0805), Q_UINT64_C(0x50B704CAB602C329),
  Q_UINT64_C(0x4CC317FB9CDDD023), Q_UINT64_C(0x66B4835D9EAFEA22), Q_UINT64_C(0x219B97E26FFC81BD),
  Q_UINT64_C(0x261E4E4C0A333A9D), Q_UINT64_C(0x1FE2CCA76517DB90), Q_UINT64_C(0xD7504DFA8816EDBB),
  Q_UINT64_C(0xB9571FA04DC089C8), Q_UINT64_C(0x1DDC0325259B27DE), Q_UINT64_C(0xCF3F4688801EB9AA),
  Q_UINT64_C(0xF4F5D05C10CAB243), Q_UINT64_C(0x38B6525C21A42B0E), Q_UINT64_C(0x36F60E2BA4FA6800),
  Q_UINT64_C(0xEB3593803173E0CE), Q_UINT64_C(0x9C4CD6257C5A3603), Q_UINT64_C(0xAF0C317D32ADAA8A),
  Q_UINT64_C(0x258E5A80C7204C4B), Q_UINT64_C(0x8B889D624D44885D), Q_UINT64_C(0xF4D14597E660F855),
  Q_UINT64_C(0xD4347F66EC8941C3), Q_UINT64_C(0xE699ED85B0DFB40D), Q_UINT64_C(0x2472F6207C2D0484),
  Q_UINT64_C(0xC2A1E7B5B459AEB5), Q_UINT64_C(0xAB4F6451CC1D45EC), Q_UINT64_C(0x63767572AE3D6174),
  Q_UINT64_C(0xA59E0BD101731A28), Q_UINT64_C(0x116D0016CB948F09), Q_UINT64_C(0x2CF9C8CA052F6E9F),
  Q_UINT64_C(0x0B090A7560A968E3), Q_UINT64_C(0xABEEDDB2DDE06FF1), Q_UINT64_C(0x58EFC10B06A2068D),
  Q_UINT64_C(0xC6E57A78FBD986E0), Q_UINT64_C(0x2EAB8CA63CE802D7), Q_UINT64_C(0x14A195640116F336),
  Q_UINT64_C(0x7C0828DD624EC390), Q_UINT64_C(0xD74BBE77E6116AC7), Q_UINT64_C(0x804456AF10F5FB53),
  Q_UINT64_C(0xEBE9EA2ADF4321C7), Q_UINT64_C(0x03219A39EE587A30), Q_UINT64_C(0x49787FEF17AF9924),
  Q_UINT64_C(0xA1E9300CD8520548), Q_UINT64_C(0x5B45E522E4B1B4EF), Q_UINT64_C(0xB49C3B3995091A36),
  Q_UINT64_C(0xD4490AD526F14431), Q_UINT64_C(0x12A8F216AF9418C2), Q_UINT64_C(0x001F837CC7350524),
  Q_UINT64_C(0x1877B51E57A764D5), Q_UINT64_C(0xA2853B80F17F58EE), Q_UINT64_C(0x993E1DE72D36D310),
  Q_UINT64_C(0xB3598080CE64A656), Q_UINT64_C(0x252F59CF0D9F04BB), Q_UINT64_C(0xD23C8E176D113600),
  Q_UINT64_C(0x1BDA0492E7E4586E), Q_UINT64_C(0x21E0BD5026C619BF), Q_UINT64_C(0x3B097ADAF088F94E),
  Q_UINT64_C(0x8D14DEDB30BE846E), Q_UINT64_C(0xF95CFFA23AF5F6F4), Q_UINT64_C(0x3871700761B3F743),
  Q_UINT64_C(0xCA672B91E9E4FA16), Q_UINT64_C(0x64C8E531BFF53B55), Q_UINT64_C(0x241260ED4AD1E87D),
  Q_UINT64_C(0x106C09B972D2E822), Q_UINT64_C(0x7FBA195410E5CA30), Q_UINT64_C(0x7884D9BC6CB569D8),
  Q_UINT64_C(0x0647DFEDCD894A29), Q_UINT64_C(0x63573FF03E224774), Q_UINT64_C(0x4FC8E9560F91B123),
  Q_UINT64_C(0x1DB956E450275779), Q_UINT64_C(0xB8D91274B9E9D4FB), Q_UINT64_C(0xA2EBEE47E2FBFCE1),
  Q_UINT64_C(0xD9F1F30CCD97FB09), Q_UINT64_C(0xEFED53D75FD64E6B), Q_UINT64_C(0x2E6D02C36017F67F),
  Q_UINT64_C(0xA9AA4D20DB084E9B), Q_UINT64_C(0xB64BE8D8B25396C1), Q_UINT64_C(0x70CB6AF7C2D5BCF0),
  Q_UINT64_C(0x98F076A4F7A2322E), Q_UINT64_C(0xBF84470805E69B5F), Q_UINT64_C(0x94C3251F06F90CF3),
  Q_UINT64_C(0x3E003E616A6591E9), Q_UINT64_C(0xB925A6CD0421AFF3), Q_UINT64_C(0x61BDD1307C66E300),
  Q_UINT64_C(0xBF8D5108E27E0D48), Q_UINT64_C(0x240AB57A8B888B20), Q_UINT64_C(0xFC87614BAF287E07),
  Q_UINT64_C(0xEF02CDD06FFDB432), Q_UINT64_C(0xA1082C0466DF6C0A), Q_UINT64_C(0x8215E577001332C8),
  Q_UINT64_C(0xD39BB9C3A48DB6CF), Q_UINT64_C(0x2738259634305C14), Q_UINT64_C(0x61CF4F94C97DF93D),
  Q_UINT64_C(0x1B6BACA2AE4E125B), Q_UINT64_C(0x758F450C88572E0B), Q_UINT64_C(0x959F587D507A8359),
  Q_UINT64_C(0xB063E962E045F54D), Q_UINT64_C(0x60E8ED72C0DFF5D1), Q_UINT64_C(0x7B64978555326F9F),
  Q_UINT64_C(0xFD080D236DA814BA), Q_UINT64_C(0x8C90FD9B083F4558), Q_UINT64_C(0x106F72FE81E2C590),
  Q_UINT64_C(0x7976033A39F7D952), Q_UINT64_C(0xA4EC0132764CA04B), Q_UINT64_C(0x733EA705FAE4FA77),
  Q_UINT64_C(0xB4D8F77BC3E56167), Q_UINT64_C(0x9E21F4F903B33FD9), Q_UINT64_C(0x9D765E419FB69F6D),
  Q_UINT64_C(0xD30C088BA61EA5EF), Q_UINT64_C(0x5D94337FBFAF7F5B), Q_UINT64_C(0x1A4E4822EB4D7A59),
  Q_UINT64_C(0x6FFE73E81B637FB3), Q_UINT64_C(0xDDF957BC36D8B9CA), Q_UINT64_C(0x64D0E29EEA8838B3),
  Q_UINT64_C(0x08DD9BDFD96B9F63), Q_UINT64_C(0x087E79E5A57D1D13), Q_UINT64_C(0xE328E230E3E2B3FB),
  Q_UINT64_C(0x1C2559E30F0946BE), Q_UINT64_C(0x720BF5F26F4D2EAA), Q_UINT64_C(0xB0774D261CC609DB),
  Q_UINT64_C(0x443F64EC5A371195), Q_UINT64_C(0x4112CF68649A260E), Q_UINT64_C(0xD813F2FAB7F5C5CA),
  Q_UINT64_C(0x660D3257380841EE), Q_UINT64_C(0x59AC2C7873F910A3), Q_UINT64_C(0xE846963877671A17),
  Q_UINT64_C(0x93B633ABFA3469F8), Q_UINT64_C(0xC0C0F5A60EF4CDCF), Q_UINT64_C(0xCAF21ECD4377B28C),
  Q_UINT64_C(0x57277707199B8175), Q_UINT64_C(0x506C11B9D90E8B1D), Q_UINT64_C(0xD83CC2687A19255F),
  Q_UINT64_C(0x4A29C6465A314CD1), Q_UINT64_C(0xED2DF21216235097), Q_UINT64_C(0xB5635C95FF7296E2),
  Q_UINT64_C(0x22AF003AB672E811), Q_UINT64_C(0x52E762596BF68235), Q_UINT64_C(0x9AEBA33AC6ECC6B0),
  Q_UINT64_C(0x944F6DE09134DFB6), Q_UINT64_C(0x6C47BEC883A7DE39), Q_UINT64_C(0x6AD047C430A12104),
  Q_UINT64_C(0xA5B1CFDBA0AB4067), Q_UINT64_C(0x7C45D833AFF07862), Q_UINT64_C(0x5092EF950A16DA0B),
  Q_UINT64_C(0x9338E69C052B8E7B), Q_UINT64_C(0x455A4B4CFE30E3F5), Q_UINT64_C(0x6B02E63195AD0CF8),
  Q_UINT64_C(0x6B17B224BAD6BF27), Q_UINT64_C(0xD1E0CCD25BB9C169), Q_UINT64_C(0xDE0C89A556B9AE70),
  Q_UINT64_C(0x50065E535A213CF6), Q_UINT64_C(0x9C1169FA2777B874), Q_UINT64_C(0x78EDEFD694AF1EED),
  Q_UINT64_C(0x6DC93D9526A50E68), Q_UINT64_C(0xEE97F453F06791ED), Q_UINT64_C(0x32AB0EDB696703D3),
  Q_UINT64_C(0x3A6853C7E70757A7), Q_UINT64_C(0x31865CED6120F37D), Q_UINT64_C(0x67FEF95D92607890),
  Q_UINT64_C(0x1F2B1D1F15F6DC9C), Q_UINT64_C(0xB69E38A8965C6B65), Q_UINT64_C(0xAA9119FF184CCCF4),
  Q_UINT64_C(0xF43C732873F24C13), Q_UINT64_C(0xFB4A3D794A9A80D2), Q_UINT64_C(0x3550C2321FD6109C),
  Q_UINT64_C(0x371F77E76BB8417E), Q_UINT64_C(0x6BFA9AAE5EC05779), Q_UINT64_C(0xCD04F3FF001A4778),
  Q_UINT64_C(0xE3273522064480CA), Q_UINT64_C(0x9F91508BFFCFC14A), Q_UINT64_C(0x049A7F41061A9E60),
  Q_UINT64_C(0xFCB6BE43A9F2FE9B), Q_UINT64_C(0x08DE8A1C7797DA9B), Q_UINT64_C(0x8F9887E6078735A1),
  Q_UINT64_C(0xB5B4071DBFC73A66), Q_UINT64_C(0x230E343DFBA08D33), Q_UINT64_C(0x43ED7F5A0FAE657D),
  Q_UINT64_C(0x3A88A0FBBCB05C63), Q_UINT64_C(0x21874B8B4D2DBC4F), Q_UINT64_C(0x1BDEA12E35F6A8C9),
  Q_UINT64_C(0x53C065C6C8E63528), Q_UINT64_C(0xE34A1D250E7A8D6B), Q_UINT64_C(0xD6B04D3B7651DD7E),
  Q_UINT64_C(0x5E90277E7CB39E2D), Q_UINT64_C(0x2C046F22062DC67D), Q_UINT64_C(0xB10BB459132D0A26),
  Q_UINT64_C(0x3FA9DDFB67E2F199), Q_UINT64_C(0x0E09B88E1914F7AF), Q_UINT64_C(0x10E8B35AF3EEAB37),
  Q_UINT64_C(0x9EEDECA8E272B933), Q_UINT64_C(0xD4C718BC4AE8AE5F), Q_UINT64_C(0x81536D601170FC20),
  Q_UINT64_C(0x91B534F885818A06), Q_UINT64_C(0xEC8177F83F900978), Q_UINT64_C(0x190E714FADA5156E),
  Q_UINT64_C(0xB592BF39B0364963), Q_UINT64_C(0x89C350C893AE7DC1), Q_UINT64_C(0xAC042E70F8B383F2),
  Q_UINT64_C(0xB49B52E587A1EE60), Q_UINT64_C(0xFB152FE3FF26DA89), Q_UINT64_C(0x3E666E6F69AE2C15),
  Q_UINT64_C(0x3B544EBE544C19F9), Q_UINT64_C(0xE805A1E290CF2456), Q_UINT64_C(0x24B33C9D7ED25117),
  Q_UINT64_C(0xE74733427B72F0C1), Q_UINT64_C(0x0A804D18B7097475), Q_UINT64_C(0x57E3306D881EDB4F),
  Q_UINT64_C(0x4AE7D6A36EB5DBCB), Q_UINT64_C(0x2D8D5432157064C8), Q_UINT64_C(0xD1E649DE1E7F268B),
  Q_UINT64_C(0x8A328A1CEDFE552C), Q_UINT64_C(0x07A3AEC79624C7DA), Q_UINT64_C(0x84547DDC3E203C94),
  Q_UINT64_C(0x990A98FD5071D263), Q_UINT64_C(0x1A4FF12616EEFC89), Q_UINT64_C(0xF6F7FD1431714200),
  Q_UINT64_C(0x30C05B1BA332F41C), Q_UINT64_C(0x8D2636B81555A786), Q_UINT64_C(0x46C9FEB55D120902),
  Q_UINT64_C(0xCCEC0A73B49C9921), Q_UINT64_C(0x4E9D2827355FC492), Q_UINT64_C(0x19EBB029435DCB0F),
  Q_UINT64_C(0x4659D2B743848A2C), Q_UINT64_C(0x963EF2C96B33BE31), Q_UINT64_C(0x74F85198B05A2E7D),
  Q_UINT64_C(0x5A0F544DD2B1FB18), Q_UINT64_C(0x03727073C2E134B1), Q_UINT64_C(0xC7F6AA2DE59AEA61),
  Q_UINT64_C(0x352787BAA0D7C22F), Q_UINT64_C(0x9853EAB63B5E0B35), Q_UINT64_C(0xABBDCDD7ED5C0860),
  Q_UINT64_C(0xCF05DAF5AC8D77B0), Q_UINT64_C(0x49CAD48CEBF4A71E), Q_UINT64_C(0x7A4C10EC2158C4A6),
  Q_UINT64_C(0xD9E92AA246BF719E), Q_UINT64_C(0x13AE978D09FE5557), Q_UINT64_C(0x730499AF921549FF),
  Q_UINT64_C(0x4E4B705B92903BA4), Q_UINT64_C(0xFF577222C14F0A3A), Q_UINT64_C(0x55B6344CF97AAFAE),
  Q_UINT64_C(0xB862225B055B6960), Q_UINT64_C(0xCAC09AFBDDD2CDB4), Q_UINT64_C(0xDAF8E9829FE96B5F),
  Q_UINT64_C(0xB5FDFC5D3132C498), Q_UINT64_C(0x310CB380DB6F7503), Q_UINT64_C(0xE87FBB46217A360E),
  Q_UINT64_C(0x2102AE466EBB1148), Q_UINT64_C(0xF8549E1A3AA5E00D), Q_UINT64_C(0x07A69AFDCC42261A),
  Q_UINT64_C(0xC4C118BFE78FEAAE), Q_UINT64_C(0xF9F4892ED96BD438), Q_UINT64_C(0x1AF3DBE25D8F45DA),
  Q_UINT64_C(0xF5B4B0B0D2DEEEB4), Q_UINT64_C(0x962ACEEFA82E1C84), Q_UINT64_C(0x046E3ECAAF453CE9),
  Q_UINT64_C(0xF05D129681949A4C), Q_UINT64_C(0x964781CE734B3C84), Q_UINT64_C(0x9C2ED44081CE5FBD),
  Q_UINT64_C(0x522E23F3925E319E), Q_UINT64_C(0x177E00F9FC32F791), Q_UINT64_C(0x2BC60A63A6F3B3F2),
  Q_UINT64_C(0x222BBFAE61725606), Q_UINT64_C(0x486289DDCC3D6780), Q_UINT64_C(0x7DC7785B8EFDFC80),
  Q_UINT64_C(0x8AF38731C02BA980), Q_UINT64_C(0x1FAB64EA29A2DDF7), Q_UINT64_C(0xE4D9429322CD065A),
  Q_UINT64_C(0x9DA058C67844F20C), Q_UINT64_C(0x24C0E332B70019B0), Q_UINT64_C(0x233003B5A6CFE6AD),
  Q_UINT64_C(0xD586BD01C5C217F6), Q_UINT64_C(0x5E5637885F29BC2B), Q_UINT64_C(0x7EBA726D8C94094B),
  Q_UINT64_C(0x0A56A5F0BFE39272), Q_UINT64_C(0xD79476A84EE20D06), Q_UINT64_C(0x9E4C1269BAA4BF37),
  Q_UINT64_C(0x17EFEE45B0DEE640), Q_UINT64_C(0x1D95B0A5FCF90BC6), Q_UINT64_C(0x93CBE0B699C2585D),
  Q_UINT64_C(0x65FA4F227A2B6D79), Q_UINT64_C(0xD5F9E858292504D5), Q_UINT64_C(0xC2B5A03F71471A6F),
  Q_UINT64_C(0x59300222B4561E00), Q_UINT64_C(0xCE2F8642CA0712DC), Q_UINT64_C(0x7CA9723FBB2E8988),
  Q_UINT64_C(0x2785338347F2BA08), Q_UINT64_C(0xC61BB3A141E50E8C), Q_UINT64_C(0x150F361DAB9DEC26),
  Q_UINT64_C(0x9F6A419D382595F4), Q_UINT64_C(0x64A53DC924FE7AC9), Q_UINT64_C(0x142DE49FFF7A7C3D),
  Q_UINT64_C(0x0C335248857FA9E7), Q_UINT64_C(0x0A9C32D5EAE45305), Q_UINT64_C(0xE6C42178C4BBB92E),
  Q_UINT64_C(0x71F1CE2490D20B07), Q_UINT64_C(0xF1BCC3D275AFE51A), Q_UINT64_C(0xE728E8C83C334074),
  Q_UINT64_C(0x96FBF83A12884624), Q_UINT64_C(0x81A1549FD6573DA5), Q_UINT64_C(0x5FA7867CAF35E149),
  Q_UINT64_C(0x56986E2EF3ED091B), Q_UINT64_C(0x917F1DD5F8886C61), Q_UINT64_C(0xD20D8C88C8FFE65F),
  Q_UINT64_C(0x31D71DCE64B2C310), Q_UINT64_C(0xF165B587DF898190), Q_UINT64_C(0xA57E6339DD2CF3A0),
  Q_UINT64_C(0x1EF6E6DBB1961EC9), Q_UINT64_C(0x70CC73D90BC26E24), Q_UINT64_C(0xE21A6B35DF0C3AD7),
  Q_UINT64_C(0x003A93D8B2806962), Q_UINT64_C(0x1C99DED33CB890A1), Q_UINT64_C(0xCF3145DE0ADD4289),
  Q_UINT64_C(0xD0E4427A5514FB72), Q_UINT64_C(0x77C621CC9FB3A483), Q_UINT64_C(0x67A34DAC4356550B),
  Q_UINT64_C(0xF8D626AAAF278509)
};

const int RANDOM_CASTLE = 768;
const int RANDOM_EN_PASSENT = 772;
const int RANDOM_TURN = 780;

// BOARD
// for board
const int EMPTY_POS[120] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};


// initial board position
const int INIT_POS[120] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x04, 0x02, 0x03, 0x05, 0x06, 0x03, 0x02, 0x04, 0xFF,
    0xFF, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
    0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF,
    0xFF, 0x84, 0x82, 0x83, 0x85, 0x86, 0x83, 0x82, 0x84, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

// attack table
// the index of this array corresponds to the distance
// between two squares of the board (note the board is
// encoded as a one dim array of size 120, where A1 = 21, H1 = 28
// A8 = 91, A8 = 98.
// the value denotes whether an enemy rook, bishop, knight, queen, king
// on one square can attack the other square. The following encoding
// is used:
// Bitposition    Piece
// 0              Knight
// 1              Bishop
// 2              Rook
// 3              Queen
// 4              King
// e.g. distance one, i.e. index 1 (=left, up, down, right square) has
// value 0x1C = MSB 00011100 LSB, i.e. king, queen, rook can
// potentially attack
const uint8_t ATTACK_TABLE[78] = {
    0x00, 0x1C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x01, 0x1a,
    0x1C, 0x1A, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x01,
    0x0C, 0x01, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00,
    0x0C, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00,
    0x0C, 0x00, 0x00, 0x00, 0x0A, 0x0a, 0x00, 0x00, 0x00, 0x00,
    0x0C, 0x00, 0x00, 0x00, 0x0a, 0x0A, 0x00, 0x00, 0x00, 0x00,
    0x0C, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00,
    0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A
};

const int IDX_BPAWN = 0;
const int IDX_WPAWN = 1;
const int IDX_KNIGHT = 2;
const int IDX_BISHOP = 3;
const int IDX_ROOK = 4;
const int IDX_QUEEN = 5;
const int IDX_KING = 6;

// first dim is for different piece types
// [piece_type[0] is DCOUNT (as in Byte Magazine paper)
// [piece_type[1] ... [piece_type][4] resp.
// [piece_type[1] ... [piece_type][8] contain
// DPOINT table
const int DIR_TABLE[7][9] = {
    { 4, -10, -20, -11, -9 ,   0,   0,   0,   0 }, // max 4 black pawn directions, rest 0's
    { 4, +10, +20, +11, +9 ,   0,   0,   0,   0 }, // max 4 white pawn directions, rest 0's
    { 8, -21, -12, +8 , +19, +21, +12, -8, -19 }, // 8 knight directions
    { 4, +9 , +11, -11, -9 ,   0,   0,   0,   0 }, // 4 bishop directions
    { 4, +10, -10, +1 , -1 ,   0,   0,   0,   0 }, // 4 rook directions
    { 8, +9 , +11, -11, -9 ,   +10, -10, +1, -1 }, // 8 queen directions
    { 8, +9 , +11, -11, -9 ,   +10, -10, +1, -1 }  // 8 king directions (= queen dir's)
};

// players
const bool WHITE = false;
const bool BLACK = true;

// bit positions of flags
const uint8_t COLOR_FLAG = 7;
//const uint8_t CASTLE_FLAG = 4;
//const uint8_t MOVED_FLAG = 4;

const QString STARTING_FEN = QString("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

// board positions
const int A1 = 21;
const int A2 = 31;
const int A3 = 41;
const int A4 = 51;
const int A5 = 61;
const int A6 = 71;
const int A7 = 81;
const int A8 = 91;

const int B1 = 22;
const int B2 = 32;
const int B3 = 42;
const int B4 = 52;
const int B5 = 62;
const int B6 = 72;
const int B7 = 82;
const int B8 = 92;

const int C1 = 23;
const int C2 = 33;
const int C3 = 43;
const int C4 = 53;
const int C5 = 63;
const int C6 = 73;
const int C7 = 83;
const int C8 = 93;

const int D1 = 24;
const int D2 = 34;
const int D3 = 44;
const int D4 = 54;
const int D5 = 64;
const int D6 = 74;
const int D7 = 84;
const int D8 = 94;

const int E1 = 25;
const int E2 = 35;
const int E3 = 45;
const int E4 = 55;
const int E5 = 65;
const int E6 = 75;
const int E7 = 85;
const int E8 = 95;

const int F1 = 26;
const int F2 = 36;
const int F3 = 46;
const int F4 = 56;
const int F5 = 66;
const int F6 = 76;
const int F7 = 86;
const int F8 = 96;

const int G1 = 27;
const int G2 = 37;
const int G3 = 47;
const int G4 = 57;
const int G5 = 67;
const int G6 = 77;
const int G7 = 87;
const int G8 = 97;

const int H1 = 28;
const int H2 = 38;
const int H3 = 48;
const int H4 = 58;
const int H5 = 68;
const int H6 = 78;
const int H7 = 88;
const int H8 = 98;

const int ANY_SQUARE = 0;

const int WHITE_KING = 0x06;
const int WHITE_QUEEN = 0x05;
const int WHITE_ROOK = 0x04;
const int WHITE_BISHOP = 0x03;
const int WHITE_KNIGHT = 0x02;
const int WHITE_PAWN = 0x01;
const int WHITE_ANY_PIECE = 0x07;

const int BLACK_KING = 0x86;
const int BLACK_QUEEN = 0x85;
const int BLACK_ROOK = 0x84;
const int BLACK_BISHOP = 0x83;
const int BLACK_KNIGHT = 0x82;
const int BLACK_PAWN = 0x81;
const int BLACK_ANY_PIECE = 0x87;

// general piece encodings
// note specific encodings
// for black/white in board.h
// general encodings have same values as WHITE
const int EMPTY = 0;
const int PAWN = 1;
const int KNIGHT = 2;
const int BISHOP = 3;
const int ROOK = 4;
const int QUEEN = 5;
const int KING = 6;

const int ANY_PIECE = 0x08;

const int FRINGE = 0xFF;

typedef std::bitset<sizeof(uint8_t)*8> IntBits;

const QRegularExpression FEN_CASTLES_REGEX = QRegularExpression("^-|[KQABCDEFGH]{0,2}[kqabcdefgh]{0,2}$");

// GAME
const int RES_UNDEF = 0;
const int RES_WHITE_WINS = 1;
const int RES_BLACK_WINS = 2;
const int RES_DRAW = 3;
const int RES_ANY = 4;

// PGN READING & WRITING
const QRegularExpression TAG_REGEX = QRegularExpression("\\[([A-Za-z0-9]+)\\s+\"(.*)\"\\]");

const int NAG_NULL = 0;

const int NAG_GOOD_MOVE = 1;
//A good move. Can also be indicated by ``!`` in PGN notation."""

const int NAG_MISTAKE = 2;
//A mistake. Can also be indicated by ``?`` in PGN notation."""

const int NAG_BRILLIANT_MOVE = 3;
//A brilliant move. Can also be indicated by ``!!`` in PGN notation."""

const int NAG_BLUNDER = 4;
//A blunder. Can also be indicated by ``??`` in PGN notation."""

const int NAG_SPECULATIVE_MOVE = 5;
//A speculative move. Can also be indicated by ``!?`` in PGN notation."""

const int NAG_DUBIOUS_MOVE = 6;
//A dubious move. Can also be indicated by ``?!`` in PGN notation."""

const int NAG_FORCED_MOVE = 7;

const int NAG_DRAWISH_POSITION = 10;

const int NAG_UNCLEAR_POSITION = 13;

const int NAG_WHITE_MODERATE_ADVANTAGE = 16;
const int NAG_BLACK_MODERATE_ADVANTAGE = 17;
const int NAG_WHITE_DECISIVE_ADVANTAGE = 18;
const int NAG_BLACK_DECISIVE_ADVANTAGE = 19;
const int NAG_WHITE_ZUGZWANG = 22;
const int NAG_BLACK_ZUGZWANG = 23;

const int NAG_WHITE_HAS_ATTACK = 40;
const int NAG_BLACK_HAS_ATTACK = 41;

const int NAG_WHITE_MODERATE_COUNTERPLAY = 132;
const int NAG_BLACK_MODERATE_COUNTERPLAY = 133;

// tokens for move parsing
const int TKN_ERROR = -1;
const int TKN_PAWN_MOVE = 1;
const int TKN_CASTLE = 2;
const int TKN_ROOK_MOVE = 3;
const int TKN_KNIGHT_MOVE = 4;
const int TKN_BISHOP_MOVE = 5;
const int TKN_QUEEN_MOVE = 6;
const int TKN_KING_MOVE = 7;
const int TKN_OPEN_VARIATION = 8;
const int TKN_CLOSE_VARIATION = 9;
const int TKN_NAG = 10;
const int TKN_OPEN_COMMENT = 11;
const int TKN_RES_WHITE_WIN = 12;
const int TKN_RES_BLACK_WIN = 13;
const int TKN_RES_DRAW = 14;
const int TKN_RES_UNDEFINED = 15;
const int TKN_CHECK = 16;
const int TKN_EOL = 17;
const int TKN_NULL_MOVE = 18;


// ERROR CODES
const int GENERAL_ERROR = -1;


}


#endif // CONSTANTS_H
