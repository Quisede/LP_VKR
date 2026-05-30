from html.parser import HTMLParser
from pathlib import Path
from zipfile import ZipFile, ZIP_DEFLATED
import html
import re


ROOT = Path(__file__).resolve().parent
HTML_PATH = ROOT / "vkr_testing_chapter.html"
DOCX_PATH = ROOT / "vkr_testing_chapter_ready.docx"


class ChapterParser(HTMLParser):
    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.blocks = []
        self.stack = []
        self.text_parts = []
        self.current_table = None
        self.current_row = None
        self.current_cell = None
        self.list_stack = []

    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        if tag in {"h1", "h2", "h3", "p", "pre", "li", "div"}:
            self.stack.append((tag, attrs))
            self.text_parts = []
        elif tag == "br":
            self.text_parts.append("\n")
        elif tag == "table":
            self.current_table = []
        elif tag == "tr":
            self.current_row = []
        elif tag in {"td", "th"}:
            self.current_cell = {"header": tag == "th", "text": []}
        elif tag in {"ul", "ol"}:
            self.list_stack.append({"tag": tag, "index": 0})
        elif tag == "code":
            self.text_parts.append("")

    def handle_endtag(self, tag):
        if tag in {"h1", "h2", "h3", "p", "pre", "li", "div"} and self.stack:
            start_tag, attrs = self.stack.pop()
            text = self.clean_text("".join(self.text_parts), preserve=tag == "pre")
            if text:
                if tag == "li":
                    marker = "•"
                    if self.list_stack:
                        current = self.list_stack[-1]
                        if current["tag"] == "ol":
                            current["index"] += 1
                            marker = f"{current['index']}."
                    self.blocks.append({"type": "list", "marker": marker, "text": text})
                elif tag == "div":
                    css_class = attrs.get("class", "")
                    if "placeholder" in css_class:
                        self.blocks.append({"type": "callout", "kind": "placeholder", "text": text})
                    elif "note" in css_class:
                        self.blocks.append({"type": "callout", "kind": "note", "text": text})
                    else:
                        self.blocks.append({"type": "p", "text": text})
                else:
                    self.blocks.append({"type": tag, "text": text})
            self.text_parts = []
        elif tag in {"td", "th"} and self.current_cell is not None:
            cell_text = self.clean_text("".join(self.current_cell["text"]))
            self.current_row.append({"text": cell_text, "header": self.current_cell["header"]})
            self.current_cell = None
        elif tag == "tr" and self.current_table is not None:
            self.current_table.append(self.current_row or [])
            self.current_row = None
        elif tag == "table" and self.current_table is not None:
            self.blocks.append({"type": "table", "rows": self.current_table})
            self.current_table = None
        elif tag in {"ul", "ol"} and self.list_stack:
            self.list_stack.pop()

    def handle_data(self, data):
        if self.current_cell is not None:
            self.current_cell["text"].append(data)
        else:
            self.text_parts.append(data)

    @staticmethod
    def clean_text(value, preserve=False):
        value = value.replace("\xa0", " ")
        if preserve:
            return value.strip()
        return re.sub(r"\s+", " ", value).strip()


def esc(value):
    return html.escape(value, quote=True)


def run_xml(text, preserve=False, bold=False, italic=False, code=False):
    props = []
    if bold:
        props.append("<w:b/>")
    if italic:
        props.append("<w:i/>")
    if code:
        props.append('<w:rFonts w:ascii="Courier New" w:hAnsi="Courier New"/>')
        props.append('<w:sz w:val="19"/>')
    rpr = f"<w:rPr>{''.join(props)}</w:rPr>" if props else ""
    space = ' xml:space="preserve"' if preserve or text.startswith(" ") or text.endswith(" ") else ""
    return f"<w:r>{rpr}<w:t{space}>{esc(text)}</w:t></w:r>"


def paragraph_xml(text, style=None, align=None, shade=None, border=False, code=False):
    ppr = []
    if style:
        ppr.append(f'<w:pStyle w:val="{style}"/>')
    if align:
        ppr.append(f'<w:jc w:val="{align}"/>')
    if shade:
        ppr.append(f'<w:shd w:fill="{shade}"/>')
    if border:
        ppr.append('<w:pBdr><w:top w:val="single" w:sz="6" w:space="3" w:color="BFDBFE"/><w:left w:val="single" w:sz="6" w:space="8" w:color="BFDBFE"/><w:bottom w:val="single" w:sz="6" w:space="3" w:color="BFDBFE"/><w:right w:val="single" w:sz="6" w:space="8" w:color="BFDBFE"/></w:pBdr>')
    ppr_xml = f"<w:pPr>{''.join(ppr)}</w:pPr>" if ppr else ""
    if code:
        runs = []
        lines = text.split("\n")
        for i, line in enumerate(lines):
            if i:
                runs.append("<w:r><w:br/></w:r>")
            runs.append(run_xml(line, preserve=True, code=True))
        return f"<w:p>{ppr_xml}{''.join(runs)}</w:p>"
    return f"<w:p>{ppr_xml}{run_xml(text)}</w:p>"


def cell_xml(text, header=False, width=3000):
    fill = '<w:shd w:fill="E8EEF7"/>' if header else ""
    tcpr = (
        f'<w:tcPr><w:tcW w:w="{width}" w:type="dxa"/>'
        '<w:tcMar><w:top w:w="100" w:type="dxa"/><w:left w:w="120" w:type="dxa"/>'
        '<w:bottom w:w="100" w:type="dxa"/><w:right w:w="120" w:type="dxa"/></w:tcMar>'
        f"{fill}</w:tcPr>"
    )
    style = "TableHeader" if header else None
    parts = [paragraph_xml(part.strip(), style=style) for part in re.split(r"\n+", text) if part.strip()]
    if not parts:
        parts = [paragraph_xml("")]
    return f"<w:tc>{tcpr}{''.join(parts)}</w:tc>"


def table_xml(rows):
    max_cols = max((len(row) for row in rows), default=1)
    width = max(1200, int(9360 / max_cols))
    grid = "".join(f'<w:gridCol w:w="{width}"/>' for _ in range(max_cols))
    body = [
        '<w:tbl><w:tblPr><w:tblStyle w:val="TableGrid"/>'
        '<w:tblW w:w="9360" w:type="dxa"/>'
        '<w:tblBorders><w:top w:val="single" w:sz="4" w:color="CBD5E1"/>'
        '<w:left w:val="single" w:sz="4" w:color="CBD5E1"/>'
        '<w:bottom w:val="single" w:sz="4" w:color="CBD5E1"/>'
        '<w:right w:val="single" w:sz="4" w:color="CBD5E1"/>'
        '<w:insideH w:val="single" w:sz="4" w:color="CBD5E1"/>'
        '<w:insideV w:val="single" w:sz="4" w:color="CBD5E1"/></w:tblBorders>'
        '</w:tblPr>',
        f"<w:tblGrid>{grid}</w:tblGrid>",
    ]
    for row in rows:
        body.append("<w:tr>")
        for cell in row:
            body.append(cell_xml(cell["text"], cell["header"], width))
        for _ in range(max_cols - len(row)):
            body.append(cell_xml("", False, width))
        body.append("</w:tr>")
    body.append("</w:tbl>")
    return "".join(body)


def build_document(blocks):
    body = []
    for block in blocks:
        kind = block["type"]
        if kind == "h1":
            body.append(paragraph_xml(block["text"], style="Title", align="center"))
        elif kind == "h2":
            body.append(paragraph_xml(block["text"], style="Heading1"))
        elif kind == "h3":
            body.append(paragraph_xml(block["text"], style="Heading2"))
        elif kind == "pre":
            body.append(paragraph_xml(block["text"], style="CodeBlock", shade="F3F4F6", border=True, code=True))
        elif kind == "list":
            body.append(paragraph_xml(f'{block["marker"]} {block["text"]}', style="ListParagraph"))
        elif kind == "callout":
            fill = "EFF6FF" if block["kind"] == "note" else "F8FAFC"
            body.append(paragraph_xml(block["text"], style="Callout", shade=fill, border=True))
        elif kind == "table":
            body.append(table_xml(block["rows"]))
        else:
            body.append(paragraph_xml(block["text"], style="Normal"))

    sect = (
        '<w:sectPr><w:pgSz w:w="12240" w:h="15840"/>'
        '<w:pgMar w:top="1440" w:right="1440" w:bottom="1440" w:left="1440" '
        'w:header="708" w:footer="708" w:gutter="0"/></w:sectPr>'
    )
    return (
        '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>'
        '<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main">'
        f"<w:body>{''.join(body)}{sect}</w:body>"
        "</w:document>"
    )


STYLES_XML = '''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:styles xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main">
  <w:style w:type="paragraph" w:default="1" w:styleId="Normal">
    <w:name w:val="Normal"/>
    <w:pPr><w:spacing w:after="160" w:line="300" w:lineRule="auto"/></w:pPr>
    <w:rPr><w:rFonts w:ascii="Arial" w:hAnsi="Arial"/><w:sz w:val="22"/></w:rPr>
  </w:style>
  <w:style w:type="paragraph" w:styleId="Title">
    <w:name w:val="Title"/>
    <w:basedOn w:val="Normal"/>
    <w:pPr><w:spacing w:after="240"/></w:pPr>
    <w:rPr><w:b/><w:rFonts w:ascii="Arial" w:hAnsi="Arial"/><w:sz w:val="36"/></w:rPr>
  </w:style>
  <w:style w:type="paragraph" w:styleId="Heading1">
    <w:name w:val="heading 1"/>
    <w:basedOn w:val="Normal"/>
    <w:pPr><w:keepNext/><w:spacing w:before="360" w:after="120"/></w:pPr>
    <w:rPr><w:b/><w:rFonts w:ascii="Arial" w:hAnsi="Arial"/><w:color w:val="0F172A"/><w:sz w:val="30"/></w:rPr>
  </w:style>
  <w:style w:type="paragraph" w:styleId="Heading2">
    <w:name w:val="heading 2"/>
    <w:basedOn w:val="Normal"/>
    <w:pPr><w:keepNext/><w:spacing w:before="240" w:after="100"/></w:pPr>
    <w:rPr><w:b/><w:rFonts w:ascii="Arial" w:hAnsi="Arial"/><w:color w:val="1F2937"/><w:sz w:val="25"/></w:rPr>
  </w:style>
  <w:style w:type="paragraph" w:styleId="ListParagraph">
    <w:name w:val="List Paragraph"/>
    <w:basedOn w:val="Normal"/>
    <w:pPr><w:ind w:left="360"/><w:spacing w:after="80" w:line="300" w:lineRule="auto"/></w:pPr>
    <w:rPr><w:rFonts w:ascii="Arial" w:hAnsi="Arial"/><w:sz w:val="22"/></w:rPr>
  </w:style>
  <w:style w:type="paragraph" w:styleId="CodeBlock">
    <w:name w:val="Code Block"/>
    <w:basedOn w:val="Normal"/>
    <w:pPr><w:spacing w:before="80" w:after="160" w:line="240" w:lineRule="auto"/></w:pPr>
    <w:rPr><w:rFonts w:ascii="Courier New" w:hAnsi="Courier New"/><w:sz w:val="19"/></w:rPr>
  </w:style>
  <w:style w:type="paragraph" w:styleId="Callout">
    <w:name w:val="Callout"/>
    <w:basedOn w:val="Normal"/>
    <w:pPr><w:spacing w:before="100" w:after="160"/></w:pPr>
    <w:rPr><w:rFonts w:ascii="Arial" w:hAnsi="Arial"/><w:sz w:val="21"/></w:rPr>
  </w:style>
  <w:style w:type="paragraph" w:styleId="TableHeader">
    <w:name w:val="Table Header"/>
    <w:basedOn w:val="Normal"/>
    <w:pPr><w:spacing w:after="0" w:line="260" w:lineRule="auto"/></w:pPr>
    <w:rPr><w:b/><w:rFonts w:ascii="Arial" w:hAnsi="Arial"/><w:sz w:val="20"/></w:rPr>
  </w:style>
</w:styles>'''


CONTENT_TYPES = '''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
  <Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>
  <Default Extension="xml" ContentType="application/xml"/>
  <Override PartName="/word/document.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/>
  <Override PartName="/word/styles.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml"/>
</Types>'''


RELS = '''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
  <Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="word/document.xml"/>
</Relationships>'''


DOC_RELS = '''<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships"/>'''


def main():
    parser = ChapterParser()
    parser.feed(HTML_PATH.read_text(encoding="utf-8"))
    document_xml = build_document(parser.blocks)
    with ZipFile(DOCX_PATH, "w", ZIP_DEFLATED) as docx:
        docx.writestr("[Content_Types].xml", CONTENT_TYPES)
        docx.writestr("_rels/.rels", RELS)
        docx.writestr("word/document.xml", document_xml)
        docx.writestr("word/styles.xml", STYLES_XML)
        docx.writestr("word/_rels/document.xml.rels", DOC_RELS)
    print(DOCX_PATH)


if __name__ == "__main__":
    main()
