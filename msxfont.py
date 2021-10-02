import fire
from PIL import Image
from PIL import ImageFont, ImageDraw
import mojimoji

from typing import List

"""
TEXT DATA
[tag:2byte][length:1byte][TEXT]...
"""


class Text:
    def __init__(self, tag: int, text: str) -> None:
        self.tag = tag
        self.text = mojimoji.han_to_zen(text)

        self.tagByteData: bytearray = bytearray(2)
        self.lengthByteData: bytearray = bytearray(1)
        self.textByteData: bytearray = None

        self.tagByteData[0] = tag % 256
        self.tagByteData[1] = tag // 256

    def setTextByteData(self, byteData: bytearray):
        self.textByteData = byteData
        if len(self.textByteData) > 255:
            print("Text length is larger then 255 .. [{}]".format(hex(self.tag)))
            self.lengthByteData[0] = len(byteData)
        else:
            self.lengthByteData[0] = len(byteData)


class TextList:
    texts: list[Text] = [
        Text(0x0001, "　台風16号の影響で伊豆諸島では1日夕方まで、関東地方の海上では1日夜のはじめ頃にかけて猛烈な風が吹き、1日夜遅くにかけて猛烈にしける所がある。暴風や高波に厳重な警戒が必要だ。関東地方では、これまでの大雨により土砂災害の危険度が非常に高まっている所があるため、土砂災害に厳重にも警戒が必要となる。"),
        Text(0x0002, "台風10/1伊豆諸島へ 対策早めに"),
        Text(0x0003, "　大型で強い台風16号は1日16時には銚子市の南東約260キロにあって、1時間におよそ35キロの速さで北東へ進んでいる。中心の気圧は950ヘクトパスカル、中心付近の最大風速は40メートル、最大瞬間風速は60メートルで暴風域を伴っている。"),
    ]

    def __init__(self) -> None:
        pass


class Glyph:
    def __init__(self, latter: str, counter: int = 0, id: int = 0) -> None:
        self.id = id
        self.latter = latter
        self.count = counter
        self.glyphByte: bytearray = bytearray(8)
        self.image = Image.new(size=(8, 8), mode="1")

        draw = ImageDraw.Draw(self.image)
        draw.rectangle((0, 0, 7, 7), fill=0)
        draw.text(
            (0, 0),
            text=self.latter,
            font=ImageFont.truetype(font="misaki_gothic_2nd.ttf", size=8),
            fill=1,
        )

        glyphPixelList = list(self.image.getdata())

        for y in range(8):
            lineByteStr: str = "0b{}{}{}{}{}{}{}{}".format(
                glyphPixelList[y * 8 + 0],
                glyphPixelList[y * 8 + 1],
                glyphPixelList[y * 8 + 2],
                glyphPixelList[y * 8 + 3],
                glyphPixelList[y * 8 + 4],
                glyphPixelList[y * 8 + 5],
                glyphPixelList[y * 8 + 6],
                glyphPixelList[y * 8 + 7],
            )
            self.glyphByte[y] = int(lineByteStr, 2)

    def setID(self, id: int):
        self.id = id

    def incCounter(self):
        self.count = self.count + 1

    def decCounter(self):
        self.count = self.count - 1

    def printGlyph(self):
        for y in range(8):
            for x in range(8):
                print("■" if self.image.getpixel((x, y)) == 1 else "□", end="")
            print(" -> {}".format(hex(self.glyphByte[y])))


class GlyphList:
    def __init__(self) -> None:
        self.list: list[Glyph] = []

    def addLatter(self, latter: str):
        if len(self.list) == 0:
            self.list.append(Glyph(latter=latter, counter=1))
            return

        for l in self.list:
            if l.latter == latter:
                # found latter
                l.incCounter()
                break
        else:
            # not found
            self.list.append(Glyph(latter, 1))

    def sortList(self):
        self.list = sorted(self.list, key=lambda latter: -latter.count)
        for index, l in enumerate(self.list):
            l.setID(index)

    def printList(self):
        print("Glyph NUM = {}".format(len(self.list)))
        for l in self.list:
            print("{} [{}] - {}".format(l.id, l.latter, l.count))
            l.printGlyph()

    def getGlyphListByteData(self) -> bytearray:
        gListByteArray: bytearray = bytearray()
        for index in range(len(self.list)):
            gListByteArray.extend(self.list[index].glyphByte)
        return gListByteArray

    def getGlyph(self, latter) -> Glyph:
        for glyph in self.list:
            if latter == glyph.latter:
                return glyph
        return None

    def getTextByteData(self, text: Text):
        data: bytearray = bytearray()

        for latter in text.text:
            glyph: Glyph = self.getGlyph(latter)
            if not glyph is None:
                for ffmulti in range(glyph.id // 255):
                    data.append(0xFF)
                data.append(glyph.id - glyph.id // 255)
            else:
                print("!! Glyph Not Found : {}".format(latter))

        text.setTextByteData(data)
        return data


class fontgenerate(object):
    def create(self, glyphfile: str = "glyph.dat", textfile: str = "text.dat"):
        # generating GlyphList
        self.textList = TextList()
        self.glyphList = GlyphList()
        for textContext in self.textList.texts:
            for index in range(len(textContext.text)):
                self.glyphList.addLatter(textContext.text[index])

        self.glyphList.sortList()
        self.glyphList.printList()

        with open(file=glyphfile, mode="wb") as fp:
            fp.write(self.glyphList.getGlyphListByteData())

        for text in self.textList.texts:
            textByteData: bytearray = self.glyphList.getTextByteData(text)
            print("{} {}".format(hex(text.tag), text.text))
            print("Length:{} Data:{}".format(len(textByteData), textByteData.hex()))

        with open(file=textfile, mode="wb") as fp:
            for text in self.textList.texts:
                fp.write(text.tagByteData)
                fp.write(text.lengthByteData)
                fp.write(text.textByteData)


if __name__ == "__main__":
    fire.Fire(fontgenerate)
