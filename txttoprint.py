
import sys

args = sys.argv
inputFileName = args[1]
outputFileName = args[2]

inputFile = open(inputFileName, "r")
outputFile = open(outputFileName, "w+")

outputFile.truncate(0)


maxLineWidth = 0

outputFile.write("{\n")

str = ""

row = 0

while True:
    line = inputFile.readline()
    maxLineWidth = max(maxLineWidth, len(line))
    if line == '':
        break
    #outputFile.write("UI::Graphics::DrawString(\"" + line.replace('\\','\\\\').rstrip() + f"\", -1, {column}, {row}, Color::Black);\n")
    codeLine = "UI::Graphics::DrawString(\"" + line.replace('\\','\\\\').rstrip() + f"\", -1, xOffset, yOffset+{row}, Color::Black);\n"
    str += codeLine
    row += 1

outputFile.write(f"int width = {maxLineWidth};\n")
outputFile.write(f"int height = {row};\n")
outputFile.write(f"int xOffset = (CurrentFrame.GetColumns() - width) / 2;\n")
outputFile.write(f"int yOffset = (CurrentFrame.GetRows() - height * 1.5) / 2;\n")

outputFile.write(str)
outputFile.write("int rows = CurrentFrame.GetRows();\nUI::Graphics::DrawString(\"DjidOS - A mediocre OS\", -1, 0, rows - 2, Color::Black);\n")

outputFile.write("}")

inputFile.close()
outputFile.close()

print(maxLineWidth)
print(row)
