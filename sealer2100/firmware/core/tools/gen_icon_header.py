from trezorlib import toif
from PIL import Image
import struct
import click
from mako import template

tmpl = '''
// clang-format off
static const uint8_t toi_icon_${name}[] = {
    // magic
    'T', 'O', 'I', '${toi.mode.value.decode()}',
    // width (16-bit), height (16-bit)
    ${length(toi.size[0])}, ${length(toi.size[1])},
    // compressed data length (32-bit)
    ${length(len(toi.data), 4)},
    // compressed data
    ${data(toi.data)}
};
'''

def length(n: int, width: int = 2) -> str:
    fmt = '<H' if width == 2 else '<L'
    s = struct.pack(fmt, n)
    return ', '.join(f'0x{b:02x}' for b in s)

def data(data: bytes) -> str:
    return ', '.join(f'0x{b:02x}' for b in data)

@click.command()
@click.argument("pth", type=click.File("rb"))
@click.argument("name", required=False, type=str, default=None)
def gen_header(pth, name):
    # if not set name, get name from path e.g. a/b/c.png -> c
    if not name:
        name = pth.name.split("/")[-1].split(".")[0]

    im = Image.open(pth)
    toi = toif.from_image(im, legacy_format=True)

    # print header content
    kwargs = {"name": name, "toi": toi, "length": length, "data": data}
    print(template.Template(tmpl).render(**kwargs))

if __name__ == "__main__":
    gen_header()
