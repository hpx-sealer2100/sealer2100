from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from typing import List
    pass

def parser_path(path: str) -> List[int]:
    # m/44'/60'/0'/0/0
    items = path.split("/")
    if items[0] == "m":
        items.remove("m")

    # check path is valid
    # path item is x' or x, x is int
    if not all(x in "'0123456789" for item in items for x in item):
        # no need do tip user here, because the path is passed by developer
        # so the developer should make sure the path is valid
        # this is just helper developer to debug
        raise ValueError("invalid path")

    def convert(item: str) -> int:
        hardened = item.endswith("'")
        if hardened:
            index = 0x80000000 | int(item[:-1])
        else:
            index = int(item)
        return index

    return list(convert(item) for item in items)
