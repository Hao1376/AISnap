"""
Convert an image file to a C header with 640×480 RGB565 uint16 array
for direct framebuffer display (stub test without camera).

Usage: python tool/jpg2c_disp.py <input_image> <output_header>
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


def generate_header(img_path, output_path):
    # Load and resize to 640×480
    img = Image.open(img_path).convert("RGB").resize((640, 480), Image.LANCZOS)
    pixels = list(img.getdata())  # list of (R, G, B) tuples
    total_pixels = len(pixels)

    print(f"Image size: {img.size}")
    print(f"Total pixels: {total_pixels}")
    print(f"Expected: 640 * 480 = {640 * 480}")

    # Convert to RGB565
    rgb565_data = [rgb888_to_rgb565(r, g, b) for r, g, b in pixels]

    # Write header
    guard = "__TEST_IMAGE_DISPLAY_H__"
    with open(output_path, "w", encoding="utf-8") as f:
        basename = os.path.basename(img_path)
        f.write("/**\n")
        f.write(f" * @file    test_image_display.h\n")
        f.write(f" * @brief   640×480 RGB565 array for LCD framebuffer display\n")
        f.write(f" *          Auto-generated from {basename}\n")
        f.write(f" *          Used for stub testing when camera is unavailable.\n")
        f.write(f" */\n")
        f.write(f"#ifndef {guard}\n")
        f.write(f"#define {guard}\n\n")
        f.write('#ifdef __cplusplus\n')
        f.write('extern "C" {\n')
        f.write("#endif\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write("/**\n")
        f.write(" * @brief  640×480 RGB565 image data for LCD framebuffer.\n")
        f.write(" *         Write directly to g_ltdc_lcd_framebuf[] using memcpy\n")
        f.write(" *         or rgblcd_color_fill() at (img_x_offset, img_y_offset).\n")
        f.write(" */\n")
        f.write("static const uint16_t test_image_display_rgb565[640 * 480] = {\n")

        # Write 8 values per line
        count = 0
        line_buf = "  "
        for val in rgb565_data:
            line_buf += f"0x{val:04x}, "
            count += 1
            if count % 8 == 0:
                f.write(line_buf + "\n")
                line_buf = "  "
        if count % 8 != 0:
            f.write(line_buf + "\n")

        f.write("};\n\n")
        f.write('#ifdef __cplusplus\n')
        f.write("}\n")
        f.write("#endif\n\n")
        f.write(f"#endif /* {guard} */\n")

    # Verify
    total_words = 640 * 480
    print(f"\nGenerated: {output_path}")
    print(f"  Total RGB565 words: {count} (expected {total_words})")
    assert count == total_words, f"Count mismatch! Got {count}, expected {total_words}"
    print("  Check passed!")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python tool/jpg2c_disp.py <input_image> <output_header>")
        sys.exit(1)

    input_path = sys.argv[1]
    output_path = sys.argv[2]

    if not os.path.exists(input_path):
        print(f"Error: Input file not found: {input_path}")
        sys.exit(1)

    generate_header(input_path, output_path)
