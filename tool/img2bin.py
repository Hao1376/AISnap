"""
Convert image to 640x480 RGB565 raw binary file for NOR Flash.
Usage: python tool/img2bin.py tool/palm.jpg tool/palm_disp.bin
"""
import sys
import os
import struct

try:
    from PIL import Image
except ImportError:
    import subprocess
    subprocess.check_call([sys.executable, "-m", "pip", "install", "pillow"])
    from PIL import Image


def rgb888_to_rgb565(r, g, b):
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)


def main():
    if len(sys.argv) < 3:
        print("Usage: python tool/img2bin.py <input_image> <output_bin>")
        sys.exit(1)

    img_path = sys.argv[1]
    bin_path = sys.argv[2]

    if not os.path.exists(img_path):
        print(f"Error: Input file not found: {img_path}")
        sys.exit(1)

    img = Image.open(img_path).convert("RGB").resize((640, 480), Image.LANCZOS)
    pixels = list(img.getdata())

    with open(bin_path, "wb") as f:
        for r, g, b in pixels:
            val = rgb888_to_rgb565(r, g, b)
            f.write(struct.pack("<H", val))  # little-endian uint16

    size = os.path.getsize(bin_path)
    expected = 640 * 480 * 2
    print(f"Generated: {bin_path}")
    print(f"  Size: {size} bytes (expected {expected})")
    assert size == expected, f"Size mismatch! Got {size}, expected {expected}"
    print("  Check passed!")


if __name__ == "__main__":
    main()
