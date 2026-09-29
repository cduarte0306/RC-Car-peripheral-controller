import argparse
import pathlib
import struct

METADATA_OFFSET = 0x8000
FLASH_ROW_SIZE = 256
FLASH_SIZE = 0x40000
APP_OFFSET = METADATA_OFFSET + FLASH_ROW_SIZE
APP_MAX_SIZE = FLASH_SIZE - APP_OFFSET
APP_STATE_VALID = 3
HEX_RECORD_SIZE = 16

crcTable = [
    0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475005,
    0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,
    0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 0x5F15ADAC, 0x5BD4B01B, 0x569796C2, 0x52568B75,
    0x6A1936C8, 0x6ED82B7F, 0x639B0DA6, 0x675A1011, 0x791D4014, 0x7DDC5DA3, 0x709F7B7A, 0x745E66CD,
    0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039, 0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5,
    0xBE2B5B58, 0xBAEA46EF, 0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033, 0xA4AD16EA, 0xA06C0B5D,
    0xD4326D90, 0xD0F37027, 0xDDB056FE, 0xD9714B49, 0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,
    0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 0xE13EF6F4, 0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D,
    0x34867077, 0x30476DC0, 0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 0x2E003DC5, 0x2AC12072,
    0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16, 0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA,
    0x7897AB07, 0x7C56B6B0, 0x71159069, 0x75D48DDE, 0x6B93DDDB, 0x6F52C06C, 0x6211E6B5, 0x66D0FB02,
    0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1, 0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
    0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B, 0xBB60ADFC, 0xB6238B25, 0xB2E29692,
    0x8AAD2B2F, 0x8E6C3698, 0x832F1041, 0x87EE0DF6, 0x99A95DF3, 0x9D684044, 0x902B669D, 0x94EA7B2A,
    0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E, 0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2,
    0xC6BCF05F, 0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34, 0xDC3ABDED, 0xD8FBA05A,
    0x690CE0EE, 0x6DCDFD59, 0x608EDB80, 0x644FC637, 0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB,
    0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 0x5C007B8A, 0x58C1663D, 0x558240E4, 0x51435D53,
    0x251D3B9E, 0x21DC2629, 0x2C9F00F0, 0x285E1D47, 0x36194D42, 0x32D850F5, 0x3F9B762C, 0x3B5A6B9B,
    0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF, 0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623,
    0xF12F560E, 0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65, 0xEBA91BBC, 0xEF68060B,
    0xD727BBB6, 0xD3E6A601, 0xDEA580D8, 0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,
    0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 0xAE3AFBA2, 0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B,
    0x9B3660C6, 0x9FF77D71, 0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD, 0x81B02D74, 0x857130C3,
    0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640, 0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C,
    0x7B827D21, 0x7F436096, 0x7200464F, 0x76C15BF8, 0x68860BFD, 0x6C47164A, 0x61043093, 0x65C52D24,
    0x119B4BE9, 0x155A565E, 0x18197087, 0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,
    0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D, 0x2056CD3A, 0x2D15EBE3, 0x29D4F654,
    0xC5A92679, 0xC1683BCE, 0xCC2B1D17, 0xC8EA00A0, 0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB, 0xDBEE767C,
    0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18, 0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4,
    0x89B8FD09, 0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662, 0x933EB0BB, 0x97FFAD0C,
    0xAFB010B1, 0xAB710D06, 0xA6322BDF, 0xA2F33668, 0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4
]

def reflect8(value):
    reflected = 0
    for bit in range(8):
        reflected |= ((value >> bit) & 1) << (7 - bit)
    return reflected


def calculate_crc(data):
    """Match xCRC32 in src/common/app_utils.c."""
    crc = 0xFFFFFFFF
    for byte in data:
        index = ((crc >> 24) ^ reflect8(byte)) & 0xFF
        crc = ((crc << 8) ^ crcTable[index]) & 0xFFFFFFFF
    return crc


def read_ihex(path):
    memory = {}
    upper_address = 0
    found_eof = False

    with path.open("r", encoding="ascii") as hex_file:
        for line_number, line in enumerate(hex_file, 1):
            line = line.strip()
            if not line:
                continue
            if not line.startswith(":"):
                raise ValueError(f"{path}:{line_number}: missing ':'")

            record = bytes.fromhex(line[1:])
            if len(record) < 5 or record[0] != len(record) - 5:
                raise ValueError(f"{path}:{line_number}: invalid record length")
            if sum(record) & 0xFF:
                raise ValueError(f"{path}:{line_number}: invalid record checksum")

            count = record[0]
            address = int.from_bytes(record[1:3], "big")
            record_type = record[3]
            data = record[4:4 + count]

            if record_type == 0x00:
                absolute_address = upper_address + address
                for offset, byte in enumerate(data):
                    memory[absolute_address + offset] = byte
            elif record_type == 0x01:
                found_eof = True
                break
            elif record_type == 0x02 and count == 2:
                upper_address = int.from_bytes(data, "big") << 4
            elif record_type == 0x04 and count == 2:
                upper_address = int.from_bytes(data, "big") << 16
            elif record_type not in (0x03, 0x05):
                raise ValueError(f"{path}:{line_number}: unsupported record type {record_type:02X}")

    if not found_eof:
        raise ValueError(f"{path}: missing EOF record")
    return memory


def ihex_record(address, data):
    header = bytes((len(data), (address >> 8) & 0xFF, address & 0xFF, 0))
    checksum = (-sum(header + data)) & 0xFF
    return ":" + (header + data + bytes((checksum,))).hex().upper()


def ihex_extended_address_record(upper_address):
    data = upper_address.to_bytes(2, "big")
    header = bytes((len(data), 0, 0, 4))
    checksum = (-sum(header + data)) & 0xFF
    return ":" + (header + data + bytes((checksum,))).hex().upper()


def write_ihex(memory, output_path):
    records = []
    current_upper_address = None
    addresses = sorted(memory)
    index = 0

    while index < len(addresses):
        address = addresses[index]
        upper_address = address >> 16
        if upper_address != current_upper_address:
            records.append(ihex_extended_address_record(upper_address))
            current_upper_address = upper_address

        chunk = bytearray((memory[address],))
        index += 1
        while (index < len(addresses) and len(chunk) < HEX_RECORD_SIZE and
               addresses[index] == address + len(chunk) and
               addresses[index] >> 16 == upper_address):
            chunk.append(memory[addresses[index]])
            index += 1

        records.append(ihex_record(address & 0xFFFF, chunk))

    records.append(":00000001FF")
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text("\n".join(records) + "\n", encoding="ascii")


def merge_ihex_files(input_paths, output_path, fill_erased_range=None):
    memory = {}
    for input_path in input_paths:
        for address, byte in read_ihex(input_path).items():
            if address in memory and memory[address] != byte:
                raise ValueError(f"conflicting HEX data at address 0x{address:08X}")
            memory[address] = byte

    if fill_erased_range is not None:
        start, end = fill_erased_range
        for address in range(start, end):
            memory.setdefault(address, 0xFF)

    write_ihex(memory, output_path)
    print(f"Combined HEX: {output_path}")


def generate_metadata(app_hex, output_path):
    flash = read_ihex(app_hex)
    app_data = {address: byte for address, byte in flash.items()
                if APP_OFFSET <= address < FLASH_SIZE}
    if APP_OFFSET not in app_data:
        raise ValueError(f"application HEX has no vector data at 0x{APP_OFFSET:04X}")

    app_end = max(app_data) + 1
    image_length = app_end - APP_OFFSET
    if not 0 < image_length <= APP_MAX_SIZE:
        raise ValueError(f"application image length {image_length} exceeds flash range")

    image = bytearray(b"\xFF") * image_length
    for address, byte in app_data.items():
        image[address - APP_OFFSET] = byte

    app_crc = calculate_crc(image)
    metadata = struct.pack("<BIII", APP_STATE_VALID, app_crc, APP_OFFSET, app_end)
    row = metadata.ljust(FLASH_ROW_SIZE, b"\xFF")

    records = [ihex_record(METADATA_OFFSET + offset,
                           row[offset:offset + HEX_RECORD_SIZE])
               for offset in range(0, FLASH_ROW_SIZE, HEX_RECORD_SIZE)]
    records.append(":00000001FF")
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text("\n".join(records) + "\n", encoding="ascii")
    print(f"Application image: 0x{APP_OFFSET:04X}-0x{app_end:05X} ({image_length} bytes)")
    print(f"Application CRC: 0x{app_crc:08X}")
    print(f"Metadata HEX: {output_path}")

if __name__ == "__main__":
    repo_root = pathlib.Path(__file__).resolve().parents[1]
    default_app_hex = repo_root / "build/src/rc-car-peripheral-controller/rc-car-peripheral-controller.cydsn/rc-car-peripheral-controller.hex"
    default_output = pathlib.Path(__file__).resolve().with_name("metadata.hex")

    parser = argparse.ArgumentParser(description="Generate bootloader metadata Intel HEX")
    parser.add_argument("--app-hex", type=pathlib.Path, default=default_app_hex)
    parser.add_argument("--output", type=pathlib.Path, default=default_output)
    parser.add_argument("--bootloader-hex", type=pathlib.Path)
    parser.add_argument("--combined-output", type=pathlib.Path)
    args = parser.parse_args()

    if (args.bootloader_hex is None) != (args.combined_output is None):
        parser.error("--bootloader-hex and --combined-output must be used together")

    generate_metadata(args.app_hex, args.output)
    if args.bootloader_hex is not None:
        app_addresses = [address for address in read_ihex(args.app_hex)
                         if APP_OFFSET <= address < FLASH_SIZE]
        app_end = max(app_addresses) + 1
        merge_ihex_files((args.bootloader_hex, args.app_hex, args.output),
                         args.combined_output, (APP_OFFSET, app_end))