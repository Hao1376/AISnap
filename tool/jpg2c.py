"""
Convert an image file to a C header with:
  1. 192×192 RGB888 uint8 array  → for AI inference input
  2. 640×480 RGB565 uint16 array → for LCD display (direct framebuffer write)

Usage: python tool/jpg2c.py <input_image> <output_header>
"""
import sys
import os

try:
    from PIL import Image
except ImportError:
    print("Pillow not installed. Installing...")
    import subprocess
    subprocess.check_call([sys.executable, "-m", "pip", "install", "pillow"])
    from PIL import Image


def rgb888_to_rgb565(r, g, b):
    """Convert 24-bit RGB888 to 16-bit RGB565."""
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)


def write_c_array(f, var_name, c_type, data_16bit, items_per_line):
    """Write a C array to file. data_16bit is a list of 16-bit values."""
    f.write(f"static const {c_type} {var_name} = {{\n")
    count = 0
    line_buf = "  "
    for val in data_16bit:
        line_buf += f"0x{val:04x}, "
        count += 1
        if count % items_per_line == 0:
            f.write(line_buf + "\n")
            line_buf = "  "
    if count % items_per_line != 0:
        f.write(line_buf + "\n")
    f.write("};\n\n")


def write_c_array_u8(f, var_name, c_type, data_bytes, items_per_line):
    """Write a uint8 C array to file. data_bytes is a list of byte values."""
    f.write(f"static const {c_type} {var_name} = {{\n")
    count = 0
    line_buf = "  "
    for val in data_bytes:
        line_buf += f"0x{val:02x}, "
        count += 1
        if count % items_per_line == 0:
            f.write(line_buf + "\n")
            line_buf = "  "
    if count % items_per_line != 0:
        f.write(line_buf + "\n")
    f.write("};\n\n")


def generate_header(img_path, output_path):
    # ── Load original image ──
    img = Image.open(img_path)
    print(f"Original size: {img.size}")

    # ============================================================
    # 1. 192×192 RGB888 for AI inference
    # ============================================================
    img_ai = img.convert("RGB").resize((192, 192), Image.LANCZOS)
    pixels_ai = list(img_ai.getdata())  # list of (R, G, B)
    print(f"AI  array: 192×192 RGB888 = {192 * 192} pixels")

    # Flatten to byte list: R, G, B, R, G, B, ...
    ai_bytes = []
    for r, g, b in pixels_ai:
        ai_bytes.extend([r, g, b])

    # ============================================================
    # 2. 640×480 RGB565 for LCD display
    # ============================================================
    img_disp = img.convert("RGB").resize((640, 480), Image.LANCZOS)
    pixels_disp = list(img_disp.getdata())
    print(f"Disp array: 640×480 RGB565 = {640 * 480} pixels")

    disp_rgb565 = []
    for r, g, b in pixels_disp:
        disp_rgb565.append(rgb888_to_rgb565(r, g, b))

    # ============================================================
    # Write header
    # ============================================================
    guard = "__TEST_IMAGE_H__"
    with open(output_path, "w", encoding="utf-8") as f:
        basename = os.path.basename(img_path)
        f.write("/**\n")
        f.write(f" * @file    test_image.h\n")
        f.write(f" * @brief   Test image arrays for palm detection stub test\n")
        f.write(f" *          Auto-generated from {basename}\n")
        f.write(f" *\n")
        f.write(f" *  1. test_image_rgb888[192*192*3]  → AI inference (RGB888)\n")
        f.write(f" *  2. test_image_display_rgb565[640*480] → LCD framebuffer (RGB565)\n")
        f.write(f" */\n")
        f.write(f"#ifndef {guard}\n")
        f.write(f"#define {guard}\n\n")
        f.write('#ifdef __cplusplus\n')
        f.write('extern "C" {\n')
        f.write("#endif\n\n")
        f.write("#include <stdint.h>\n\n")

        # ── Array 1: 192×192 RGB888 ──
        f.write("/* 192×192 RGB888 — AI inference input */\n")
        write_c_array_u8(f,
                         "test_image_rgb888[192 * 192 * 3]",
                         "unsigned char",
                         ai_bytes, 12)

        # ── Array 2: 640×480 RGB565 ──
        f.write("/* 640×480 RGB565 — LCD display (direct framebuffer write) */\n")
        write_c_array(f,
                      "test_image_display_rgb565[640 * 480]",
                      "uint16_t",
                      disp_rgb565, 8)

        f.write('#ifdef __cplusplus\n')
        f.write("}\n")
        f.write("#endif\n\n")
        f.write(f"#endif /* {guard} */\n")

    # ── Verify ──
    ai_total = 192 * 192 * 3
    disp_total = 640 * 480
    print(f"\nGenerated: {output_path}")
    print(f"  192×192 RGB888 : {len(ai_bytes)} bytes (expected {ai_total})")
    print(f"  640×480 RGB565 : {len(disp_rgb565)} words × 2B = {len(disp_rgb565) * 2} bytes (expected {disp_total} words)")
    assert len(ai_bytes) == ai_total, f"AI byte count mismatch! Got {len(ai_bytes)}, expected {ai_total}"
    assert len(disp_rgb565) == disp_total, f"Display word count mismatch! Got {len(disp_rgb565)}, expected {disp_total}"
    print("All checks passed!")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python jpg2c.py <input_image> <output_header>")
        sys.exit(1)

    input_path = sys.argv[1]
    output_path = sys.argv[2]

    if not os.path.exists(input_path):
        print(f"Error: Input file not found: {input_path}")
        sys.exit(1)

    generate_header(input_path, output_path)
