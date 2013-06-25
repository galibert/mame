#include "emu.h"
#include "la32.h"

DEFINE_DEVICE_TYPE(LA32, la32_device, "la32", "LA32")

// These 3 tables are present on the die.  They're the only tables in there
// 2**((n+1)/512) * 8192
const uint16_t la32_device::exp2_table[0x200] = {
	0xff5, 0xfea, 0xfdf, 0xfd4, 0xfc9, 0xfbe, 0xfb3, 0xfa8, 0xf9d, 0xf92, 0xf87, 0xf7c, 0xf71, 0xf66, 0xf5b, 0xf50,
	0xf46, 0xf3b, 0xf30, 0xf25, 0xf1a, 0xf10, 0xf05, 0xefa, 0xeef, 0xee5, 0xeda, 0xecf, 0xec5, 0xeba, 0xeaf, 0xea5,
	0xe9a, 0xe8f, 0xe85, 0xe7a, 0xe70, 0xe65, 0xe5b, 0xe50, 0xe46, 0xe3b, 0xe31, 0xe26, 0xe1c, 0xe11, 0xe07, 0xdfd,
	0xdf2, 0xde8, 0xddd, 0xdd3, 0xdc9, 0xdbe, 0xdb4, 0xdaa, 0xda0, 0xd95, 0xd8b, 0xd81, 0xd77, 0xd6c, 0xd62, 0xd58,
	0xd4e, 0xd44, 0xd3a, 0xd30, 0xd25, 0xd1b, 0xd11, 0xd07, 0xcfd, 0xcf3, 0xce9, 0xcdf, 0xcd5, 0xccb, 0xcc1, 0xcb7,
	0xcad, 0xca3, 0xc99, 0xc8f, 0xc86, 0xc7c, 0xc72, 0xc68, 0xc5e, 0xc54, 0xc4a, 0xc41, 0xc37, 0xc2d, 0xc23, 0xc1a,
	0xc10, 0xc06, 0xbfc, 0xbf3, 0xbe9, 0xbdf, 0xbd6, 0xbcc, 0xbc2, 0xbb9, 0xbaf, 0xba6, 0xb9c, 0xb93, 0xb89, 0xb7f,
	0xb76, 0xb6c, 0xb63, 0xb59, 0xb50, 0xb47, 0xb3d, 0xb34, 0xb2a, 0xb21, 0xb17, 0xb0e, 0xb05, 0xafb, 0xaf2, 0xae9,
	0xadf, 0xad6, 0xacd, 0xac3, 0xaba, 0xab1, 0xaa8, 0xa9e, 0xa95, 0xa8c, 0xa83, 0xa7a, 0xa70, 0xa67, 0xa5e, 0xa55,
	0xa4c, 0xa43, 0xa3a, 0xa31, 0xa28, 0xa1e, 0xa15, 0xa0c, 0xa03, 0x9fa, 0x9f1, 0x9e8, 0x9df, 0x9d6, 0x9ce, 0x9c5,
	0x9bc, 0x9b3, 0x9aa, 0x9a1, 0x998, 0x98f, 0x986, 0x97e, 0x975, 0x96c, 0x963, 0x95a, 0x951, 0x949, 0x940, 0x937,
	0x92e, 0x926, 0x91d, 0x914, 0x90c, 0x903, 0x8fa, 0x8f2, 0x8e9, 0x8e0, 0x8d8, 0x8cf, 0x8c7, 0x8be, 0x8b5, 0x8ad,
	0x8a4, 0x89c, 0x893, 0x88b, 0x882, 0x87a, 0x871, 0x869, 0x860, 0x858, 0x850, 0x847, 0x83f, 0x836, 0x82e, 0x826,
	0x81d, 0x815, 0x80c, 0x804, 0x7fc, 0x7f4, 0x7eb, 0x7e3, 0x7db, 0x7d2, 0x7ca, 0x7c2, 0x7ba, 0x7b1, 0x7a9, 0x7a1,
	0x799, 0x791, 0x789, 0x780, 0x778, 0x770, 0x768, 0x760, 0x758, 0x750, 0x748, 0x740, 0x738, 0x730, 0x727, 0x71f,
	0x717, 0x70f, 0x707, 0x6ff, 0x6f8, 0x6f0, 0x6e8, 0x6e0, 0x6d8, 0x6d0, 0x6c8, 0x6c0, 0x6b8, 0x6b0, 0x6a8, 0x6a1,
	0x699, 0x691, 0x689, 0x681, 0x67a, 0x672, 0x66a, 0x662, 0x65a, 0x653, 0x64b, 0x643, 0x63c, 0x634, 0x62c, 0x624,
	0x61d, 0x615, 0x60e, 0x606, 0x5fe, 0x5f7, 0x5ef, 0x5e7, 0x5e0, 0x5d8, 0x5d1, 0x5c9, 0x5c2, 0x5ba, 0x5b3, 0x5ab,
	0x5a4, 0x59c, 0x595, 0x58d, 0x586, 0x57e, 0x577, 0x56f, 0x568, 0x560, 0x559, 0x552, 0x54a, 0x543, 0x53c, 0x534,
	0x52d, 0x525, 0x51e, 0x517, 0x510, 0x508, 0x501, 0x4fa, 0x4f2, 0x4eb, 0x4e4, 0x4dd, 0x4d5, 0x4ce, 0x4c7, 0x4c0,
	0x4b9, 0x4b1, 0x4aa, 0x4a3, 0x49c, 0x495, 0x48e, 0x487, 0x480, 0x478, 0x471, 0x46a, 0x463, 0x45c, 0x455, 0x44e,
	0x447, 0x440, 0x439, 0x432, 0x42b, 0x424, 0x41d, 0x416, 0x40f, 0x408, 0x401, 0x3fa, 0x3f3, 0x3ec, 0x3e6, 0x3df,
	0x3d8, 0x3d1, 0x3ca, 0x3c3, 0x3bc, 0x3b5, 0x3af, 0x3a8, 0x3a1, 0x39a, 0x393, 0x38d, 0x386, 0x37f, 0x378, 0x372,
	0x36b, 0x364, 0x35d, 0x357, 0x350, 0x349, 0x343, 0x33c, 0x335, 0x32f, 0x328, 0x321, 0x31b, 0x314, 0x30e, 0x307,
	0x300, 0x2fa, 0x2f3, 0x2ed, 0x2e6, 0x2e0, 0x2d9, 0x2d3, 0x2cc, 0x2c5, 0x2bf, 0x2b8, 0x2b2, 0x2ac, 0x2a5, 0x29f,
	0x298, 0x292, 0x28b, 0x285, 0x27e, 0x278, 0x272, 0x26b, 0x265, 0x25f, 0x258, 0x252, 0x24b, 0x245, 0x23f, 0x238,
	0x232, 0x22c, 0x226, 0x21f, 0x219, 0x213, 0x20c, 0x206, 0x200, 0x1fa, 0x1f4, 0x1ed, 0x1e7, 0x1e1, 0x1db, 0x1d5,
	0x1ce, 0x1c8, 0x1c2, 0x1bc, 0x1b6, 0x1b0, 0x1a9, 0x1a3, 0x19d, 0x197, 0x191, 0x18b, 0x185, 0x17f, 0x179, 0x173,
	0x16d, 0x167, 0x161, 0x15b, 0x155, 0x14f, 0x149, 0x143, 0x13d, 0x137, 0x131, 0x12b, 0x125, 0x11f, 0x119, 0x113,
	0x10d, 0x107, 0x101, 0x0fb, 0x0f6, 0x0f0, 0x0ea, 0x0e4, 0x0de, 0x0d8, 0x0d2, 0x0cd, 0x0c7, 0x0c1, 0x0bb, 0x0b5,
	0x0b0, 0x0aa, 0x0a4, 0x09e, 0x098, 0x093, 0x08d, 0x087, 0x082, 0x07c, 0x076, 0x070, 0x06b, 0x065, 0x05f, 0x05a,
	0x054, 0x04e, 0x049, 0x043, 0x03d, 0x038, 0x032, 0x02d, 0x027, 0x021, 0x01c, 0x016, 0x011, 0x00b, 0x006, 0x000,
};

// exp2_table[n-1] - exp2_table[n] except for the last three values
const uint8_t la32_device::exp2_delta_table[0x200] = {
	0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb, 0xb,
	0xa, 0xb, 0xb, 0xb, 0xb, 0xa, 0xb, 0xb, 0xb, 0xa, 0xb, 0xb, 0xa, 0xb, 0xb, 0xa,
	0xb, 0xb, 0xa, 0xb, 0xa, 0xb, 0xa, 0xb, 0xa, 0xb, 0xa, 0xb, 0xa, 0xb, 0xa, 0xa,
	0xb, 0xa, 0xb, 0xa, 0xa, 0xb, 0xa, 0xa, 0xa, 0xb, 0xa, 0xa, 0xa, 0xb, 0xa, 0xa,
	0xa, 0xa, 0xa, 0xa, 0xb, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa,
	0xa, 0xa, 0xa, 0xa, 0x9, 0xa, 0xa, 0xa, 0xa, 0xa, 0xa, 0x9, 0xa, 0xa, 0xa, 0x9,
	0xa, 0xa, 0xa, 0x9, 0xa, 0xa, 0x9, 0xa, 0xa, 0x9, 0xa, 0x9, 0xa, 0x9, 0xa, 0xa,
	0x9, 0xa, 0x9, 0xa, 0x9, 0x9, 0xa, 0x9, 0xa, 0x9, 0xa, 0x9, 0x9, 0xa, 0x9, 0x9,
	0xa, 0x9, 0x9, 0xa, 0x9, 0x9, 0x9, 0xa, 0x9, 0x9, 0x9, 0x9, 0xa, 0x9, 0x9, 0x9,
	0x9, 0x9, 0x9, 0x9, 0x9, 0xa, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x8, 0x9,
	0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x9, 0x8, 0x9, 0x9, 0x9, 0x9, 0x9, 0x8, 0x9, 0x9,
	0x9, 0x8, 0x9, 0x9, 0x8, 0x9, 0x9, 0x8, 0x9, 0x9, 0x8, 0x9, 0x8, 0x9, 0x9, 0x8,
	0x9, 0x8, 0x9, 0x8, 0x9, 0x8, 0x9, 0x8, 0x9, 0x8, 0x8, 0x9, 0x8, 0x9, 0x8, 0x8,
	0x9, 0x8, 0x9, 0x8, 0x8, 0x8, 0x9, 0x8, 0x8, 0x9, 0x8, 0x8, 0x8, 0x9, 0x8, 0x8,
	0x8, 0x8, 0x8, 0x9, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x9, 0x8,
	0x8, 0x8, 0x8, 0x8, 0x7, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x7,
	0x8, 0x8, 0x8, 0x8, 0x7, 0x8, 0x8, 0x8, 0x8, 0x7, 0x8, 0x8, 0x7, 0x8, 0x8, 0x8,
	0x7, 0x8, 0x7, 0x8, 0x8, 0x7, 0x8, 0x8, 0x7, 0x8, 0x7, 0x8, 0x7, 0x8, 0x7, 0x8,
	0x7, 0x8, 0x7, 0x8, 0x7, 0x8, 0x7, 0x8, 0x7, 0x8, 0x7, 0x7, 0x8, 0x7, 0x7, 0x8,
	0x7, 0x8, 0x7, 0x7, 0x7, 0x8, 0x7, 0x7, 0x8, 0x7, 0x7, 0x7, 0x8, 0x7, 0x7, 0x7,
	0x7, 0x8, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x8, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7,
	0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x6, 0x7,
	0x7, 0x7, 0x7, 0x7, 0x7, 0x7, 0x6, 0x7, 0x7, 0x7, 0x7, 0x6, 0x7, 0x7, 0x7, 0x6,
	0x7, 0x7, 0x7, 0x6, 0x7, 0x7, 0x6, 0x7, 0x7, 0x6, 0x7, 0x7, 0x6, 0x7, 0x6, 0x7,
	0x7, 0x6, 0x7, 0x6, 0x7, 0x6, 0x7, 0x6, 0x7, 0x7, 0x6, 0x7, 0x6, 0x6, 0x7, 0x6,
	0x7, 0x6, 0x7, 0x6, 0x7, 0x6, 0x6, 0x7, 0x6, 0x6, 0x7, 0x6, 0x7, 0x6, 0x6, 0x7,
	0x6, 0x6, 0x6, 0x7, 0x6, 0x6, 0x7, 0x6, 0x6, 0x6, 0x6, 0x7, 0x6, 0x6, 0x6, 0x6,
	0x7, 0x6, 0x6, 0x6, 0x6, 0x6, 0x7, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6,
	0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6,
	0x6, 0x6, 0x6, 0x6, 0x5, 0x6, 0x6, 0x6, 0x6, 0x6, 0x6, 0x5, 0x6, 0x6, 0x6, 0x6,
	0x5, 0x6, 0x6, 0x6, 0x6, 0x5, 0x6, 0x6, 0x5, 0x6, 0x6, 0x6, 0x5, 0x6, 0x6, 0x5,
	0x6, 0x6, 0x5, 0x6, 0x6, 0x5, 0x6, 0x5, 0x6, 0x6, 0x5, 0x6, 0x5, 0x6, 0x4, 0x5,
};

// -1024*log2(sin(n+0.5)*pi/1024) except for n=0
const uint16_t la32_device::logsin_table[0x200] = {
	0x1fff, 0x1f0e, 0x1c1b, 0x1a2a, 0x18b7, 0x178e, 0x1698, 0x15c4, 0x150b, 0x1467, 0x13d3, 0x134d, 0x12d2, 0x1260, 0x11f7, 0x1194,
	0x1138, 0x10e1, 0x108f, 0x1041, 0x0ff8, 0x0fb1, 0x0f6e, 0x0f2e, 0x0ef1, 0x0eb6, 0x0e7d, 0x0e47, 0x0e12, 0x0ddf, 0x0dae, 0x0d7e,
	0x0d50, 0x0d24, 0x0cf8, 0x0cce, 0x0ca6, 0x0c7e, 0x0c57, 0x0c31, 0x0c0d, 0x0be9, 0x0bc6, 0x0ba4, 0x0b82, 0x0b62, 0x0b42, 0x0b23,
	0x0b04, 0x0ae6, 0x0ac9, 0x0aac, 0x0a90, 0x0a74, 0x0a59, 0x0a3f, 0x0a24, 0x0a0b, 0x09f2, 0x09d9, 0x09c0, 0x09a9, 0x0991, 0x097a,
	0x0963, 0x094d, 0x0937, 0x0921, 0x090b, 0x08f6, 0x08e2, 0x08cd, 0x08b9, 0x08a5, 0x0891, 0x087e, 0x086b, 0x0858, 0x0845, 0x0833,
	0x0821, 0x080f, 0x07fe, 0x07ec, 0x07db, 0x07ca, 0x07b9, 0x07a9, 0x0798, 0x0788, 0x0778, 0x0768, 0x0759, 0x0749, 0x073a, 0x072b,
	0x071c, 0x070d, 0x06fe, 0x06f0, 0x06e2, 0x06d4, 0x06c6, 0x06b8, 0x06aa, 0x069c, 0x068f, 0x0682, 0x0674, 0x0667, 0x065a, 0x064e,
	0x0641, 0x0634, 0x0628, 0x061c, 0x0610, 0x0603, 0x05f7, 0x05ec, 0x05e0, 0x05d4, 0x05c9, 0x05bd, 0x05b2, 0x05a7, 0x059c, 0x0591,
	0x0586, 0x057b, 0x0570, 0x0565, 0x055b, 0x0550, 0x0546, 0x053c, 0x0531, 0x0527, 0x051d, 0x0513, 0x0509, 0x04ff, 0x04f6, 0x04ec,
	0x04e2, 0x04d9, 0x04d0, 0x04c6, 0x04bd, 0x04b4, 0x04ab, 0x04a1, 0x0498, 0x048f, 0x0487, 0x047e, 0x0475, 0x046c, 0x0464, 0x045b,
	0x0453, 0x044a, 0x0442, 0x043a, 0x0431, 0x0429, 0x0421, 0x0419, 0x0411, 0x0409, 0x0401, 0x03f9, 0x03f2, 0x03ea, 0x03e2, 0x03db,
	0x03d3, 0x03cc, 0x03c4, 0x03bd, 0x03b5, 0x03ae, 0x03a7, 0x03a0, 0x0398, 0x0391, 0x038a, 0x0383, 0x037c, 0x0375, 0x036f, 0x0368,
	0x0361, 0x035a, 0x0353, 0x034d, 0x0346, 0x0340, 0x0339, 0x0333, 0x032c, 0x0326, 0x031f, 0x0319, 0x0313, 0x030d, 0x0306, 0x0300,
	0x02fa, 0x02f4, 0x02ee, 0x02e8, 0x02e2, 0x02dc, 0x02d6, 0x02d1, 0x02cb, 0x02c5, 0x02bf, 0x02ba, 0x02b4, 0x02ae, 0x02a9, 0x02a3,
	0x029e, 0x0298, 0x0293, 0x028d, 0x0288, 0x0282, 0x027d, 0x0278, 0x0273, 0x026d, 0x0268, 0x0263, 0x025e, 0x0259, 0x0254, 0x024f,
	0x024a, 0x0245, 0x0240, 0x023b, 0x0236, 0x0231, 0x022c, 0x0228, 0x0223, 0x021e, 0x0219, 0x0215, 0x0210, 0x020b, 0x0207, 0x0202,
	0x01fe, 0x01f9, 0x01f5, 0x01f0, 0x01ec, 0x01e7, 0x01e3, 0x01df, 0x01da, 0x01d6, 0x01d2, 0x01ce, 0x01c9, 0x01c5, 0x01c1, 0x01bd,
	0x01b9, 0x01b5, 0x01b1, 0x01ad, 0x01a9, 0x01a5, 0x01a1, 0x019d, 0x0199, 0x0195, 0x0191, 0x018d, 0x018a, 0x0186, 0x0182, 0x017e,
	0x017a, 0x0177, 0x0173, 0x016f, 0x016c, 0x0168, 0x0165, 0x0161, 0x015e, 0x015a, 0x0157, 0x0153, 0x0150, 0x014c, 0x0149, 0x0145,
	0x0142, 0x013f, 0x013b, 0x0138, 0x0135, 0x0132, 0x012e, 0x012b, 0x0128, 0x0125, 0x0122, 0x011e, 0x011b, 0x0118, 0x0115, 0x0112,
	0x010f, 0x010c, 0x0109, 0x0106, 0x0103, 0x0100, 0x00fd, 0x00fb, 0x00f8, 0x00f5, 0x00f2, 0x00ef, 0x00ec, 0x00ea, 0x00e7, 0x00e4,
	0x00e1, 0x00df, 0x00dc, 0x00d9, 0x00d7, 0x00d4, 0x00d1, 0x00cf, 0x00cc, 0x00ca, 0x00c7, 0x00c5, 0x00c2, 0x00c0, 0x00bd, 0x00bb,
	0x00b8, 0x00b6, 0x00b4, 0x00b1, 0x00af, 0x00ad, 0x00aa, 0x00a8, 0x00a6, 0x00a3, 0x00a1, 0x009f, 0x009d, 0x009b, 0x0098, 0x0096,
	0x0094, 0x0092, 0x0090, 0x008e, 0x008c, 0x008a, 0x0088, 0x0086, 0x0084, 0x0082, 0x0080, 0x007e, 0x007c, 0x007a, 0x0078, 0x0076,
	0x0074, 0x0072, 0x0070, 0x006e, 0x006d, 0x006b, 0x0069, 0x0067, 0x0066, 0x0064, 0x0062, 0x0060, 0x005f, 0x005d, 0x005b, 0x005a,
	0x0058, 0x0057, 0x0055, 0x0053, 0x0052, 0x0050, 0x004f, 0x004d, 0x004c, 0x004a, 0x0049, 0x0047, 0x0046, 0x0045, 0x0043, 0x0042,
	0x0040, 0x003f, 0x003e, 0x003c, 0x003b, 0x003a, 0x0038, 0x0037, 0x0036, 0x0035, 0x0033, 0x0032, 0x0031, 0x0030, 0x002f, 0x002e,
	0x002c, 0x002b, 0x002a, 0x0029, 0x0028, 0x0027, 0x0026, 0x0025, 0x0024, 0x0023, 0x0022, 0x0021, 0x0020, 0x001f, 0x001e, 0x001d,
	0x001c, 0x001b, 0x001a, 0x001a, 0x0019, 0x0018, 0x0017, 0x0016, 0x0016, 0x0015, 0x0014, 0x0013, 0x0013, 0x0012, 0x0011, 0x0010,
	0x0010, 0x000f, 0x000e, 0x000e, 0x000d, 0x000d, 0x000c, 0x000b, 0x000b, 0x000a, 0x000a, 0x0009, 0x0009, 0x0008, 0x0008, 0x0007,
	0x0007, 0x0006, 0x0006, 0x0006, 0x0005, 0x0005, 0x0005, 0x0004, 0x0004, 0x0004, 0x0003, 0x0003, 0x0003, 0x0002, 0x0002, 0x0002,
	0x0002, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};

la32_device::la32_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, LA32, tag, owner, clock),
	  device_sound_interface(mconfig, *this),
	  irq_cb(*this),
	  sample_cb(*this)
{
}


void la32_device::device_start()
{
	irq_cb.resolve_safe();
	sample_cb.resolve_safe();
	stream = stream_alloc(0, 4, 32000);
	timer = timer_alloc(0);
	timer->adjust(attotime::from_hz(32000*2), 0, attotime::from_hz(32000*2));
}

void la32_device::device_reset()
{
	timer_phase = false;
	irq_count = 0;
	for(int i=0; i<32; i++)
		partials[i].reset();
}

void la32_device::ramp_t::reset()
{
	cur_value = 0;
	increment = 0;
	target = 0;
	done = true;
}

bool la32_device::ramp_t::check_done() const
{
	uint8_t value = cur_value >> 18;
	if(increment & 0x80)
		return value <= target;
	else
		return value >= target;
}

bool la32_device::ramp_t::step()
{
	if(done || !(increment & 0x7f))
		return false;
	if(!check_done()) {
		uint32_t stepval = 0;
		if(increment & 0x7f) {
			stepval = (0x40000 | (exp2_table[(~((increment & 7) << 6)) & 0x1ff] << 6)) >> (15 -  ((increment >> 3) & 15));
			if(increment & 0x80)
				stepval = ~stepval;
			cur_value = (cur_value + stepval) & 0x3ffffff;
		}
	}
	if(check_done()) {
		cur_value = target << 18;
		done = true;
		return true;
	}
	return false;
}

void la32_device::ramp_t::set_increment(uint8_t i)
{
	increment = i;
}

void la32_device::ramp_t::set_target(uint8_t t)
{
	target = t;
	done = false;
}

void la32_device::partial_t::reset()
{
	base_adr_cutoff = 0;
	flags = 0;
	len_flags = 0;
	pulse_width = 0;
	pitch = 0;
	tva.reset();
	tvf.reset();
	irq_ramp = irq_sample = false;
}

std::string la32_device::tts(attotime t)
{
	char buf[256];
	int nsec = t.attoseconds() / ATTOSECONDS_PER_NANOSECOND;
	sprintf(buf, "%4d.%03d,%03d,%03d", int(t.seconds()), nsec/1000000, (nsec/1000)%1000, nsec % 1000);
	return buf;
}

std::string la32_device::ttsn()
{
	return tts(machine().time());
}

void la32_device::sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples)
{
	//	logerror(" stream update %d\n", ttsn().c_str(), samples);
	for(int i=0; i<4; i++)
		memset(outputs[i], 0, samples*sizeof(stream_sample_t));
}

uint16_t la32_device::compute_output_pcm(partial_t &p)
{
	if(p.pitch != 0xffff)
		fprintf(stderr, "Pitch %04x\n", p.pitch);
	return 0;
}

uint16_t la32_device::compute_output_synthetic(partial_t &p)
{
	return 0;
}

void la32_device::step_ramp(partial_t &p, ramp_t &r)
{
	if(r.step() && !p.irq_ramp) {
		p.irq_ramp = true;
		irq_count++;
		if(irq_count == 1)
			irq_cb(true);
	}
}

uint16_t la32_device::compute_output(partial_t &p)
{
	if(!p.active())
		return 0;
	step_ramp(p, p.tva);
	step_ramp(p, p.tvf);
	if(p.pcm())
		return compute_output_pcm(p);
	else
		return compute_output_synthetic(p);
}

void la32_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	timer_phase = !timer_phase;
	sample_cb(timer_phase);
	if(timer_phase) {
		for(int i=0; i<32; i+=2) {
		  //			uint16_t o0 = compute_output(partials[i]);
		  //			uint16_t o1 = compute_output(partials[i+1]);
		}
	}
}

READ8_MEMBER(la32_device::read)
{
	// On 400?
	// bit 7 = 0, low bits = partial+1, end of sample
	// bit 7 = 1, bit 5 = 0, ?
	// bit 7 = 1, bit 5 = 1, ramp done

	offset /= 2;
	uint8_t res = 0xff;
	for(int i=0; res == 0xff && i<32; i++) {
		if(partials[i].irq_sample) {
			partials[i].irq_sample = false;
			res = (i+1) & 0x1f;
			irq_count--;

		} else if(partials[i].irq_ramp) {
			partials[i].irq_ramp = false;
			res = 0xc0 | i;
			irq_count--;
		}
	}

	if(res != 0xff && !irq_count)
		irq_cb(false);
	
	logerror("read  %03x %02x\n", offset, res);
	return res;
}

WRITE8_MEMBER(la32_device::write)
{
	offset /= 2;

	if(offset & 0x400) {
		int partial = (offset >> 1) & 31;
		partial_t &p = partials[partial];
		int slot = ((offset >> 5) & 0xe) | (offset & 1);

		switch(slot) {
		case 0x0: // c00
			p.tva.set_increment(data);
			break;

		case 0x1: // c02
			p.tva.set_target(data);
			logerror("%02x: tva = (%02x, %02x)\n", partial, p.tva.target, p.tva.increment);
			break;

		case 0x2: // c80
			p.pulse_width = data;
			logerror("%02x: pulse_width = %02x\n", partial, p.pulse_width);
			break;

		case 0x3: // c82
			p.base_adr_cutoff = data;
			if(p.pcm())
				logerror("%02x: pcm base adr = %x\n", partial, p.base_adr());
			else
				logerror("%02x: base cutoff = %x\n", partial, p.base_cutoff());
			break;

		case 0x4: // d00
			p.tvf.set_increment(data);
			break;

		case 0x5: // d02
			p.tvf.set_target(data);
			logerror("%02x: tvf = (%02x, %02x)\n", partial, p.tvf.target, p.tvf.increment);
			break;

		case 0x6: // d80
			p.pitch = (p.pitch & 0xff00) | data;
			break;

		case 0x7: // d82
			p.pitch = (p.pitch & 0x00ff) | (data << 8);
			logerror("%02x: pitch = %x\n", partial, p.pitch);
			break;

		case 0x8: // e00
			p.flags = data;
			if(p.active())
				logerror("%02x: %s, %s=%d, ringmod=%d, chan=%d, pan=%d\n", partial, p.pcm() ? "pcm" : "synthetic", p.pcm() ? "interp" : "saw", p.pcm() ? p.interpolate() : p.sawtooth(), p.ring_modulation(), p.channel(), p.pan());
			else
				logerror("%02x: disabled\n", partial);
			break;

		case 0x9: // e02
			p.len_flags = data;
			if(p.pcm())
				logerror("%02x: len=%x, loop=%d\n", partial, p.len(), p.loop());
			else
				logerror("%02x: resonance=%02x\n", partial, p.resonance());
			break;

			// a e80
			// b e82
			// c f00
			// d f02
			// e f80
			// f f82

		default:
			logerror("write %02x.%02x, %02x\n", partial, slot, data);
			break;
		}

	} else
		logerror("write %03x, %02x\n", offset, data);
}

