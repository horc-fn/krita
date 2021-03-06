'''
A script that converts the palette named "Default" to a Gimp palette.
This ideally needs some gui and the like to select the palette to export..
By Wolthera.
'''


# Importing the relevant dependancies:
import sys
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
import math
from krita import *

allPalettes = Application.resources("palette")
paletteName = "Default"
self.currentPalette = Palette(allPalettes["paletteName"])
# open the appropriate file...
gplFile = open(paletteName+".gpl", "w")
gplFile.write("GIMP Palette\n")
gplFile.write("Name: "+paletteName+"\n")
gplFile.write("Columns: "+str(self.currentPalette.columnCount())+"\n")
gplFile.write("#"+self.currentPalette.comment()+"\n")
colorCount = self.currentPalette.colorsCountGroup("")
        
for i in range(colorCount):
    entry = self.currentPalette.colorSetEntryFromGroup(i, "")
    color = self.currentPalette.colorForEntry(entry)
    #convert to sRGB
    color.setColorSpace("RGBA", "U8", "sRGB built-in")
    
    red   = max(min(int(color.componentsOrdered()[0]*255), 255), 0)
    green = max(min(int(color.componentsOrdered()[1]*255), 255), 0)
    blue  = max(min(int(color.componentsOrdered()[2]*255), 255), 0)
    gplFile.write(str(red)+" "+str(green)+" "+str(blue)+"    "+entry.id+"-"+entry.name+"\n")
    groupNames = self.currentPalette.groupNames()
    for groupName in groupNames:
        colorCount = self.currentPalette.colorsCountGroup(groupName)
        for i in range(colorCount):
            entry = self.currentPalette.colorSetEntryFromGroup(i, groupName)
            color = self.currentPalette.colorForEntry(entry)
           #convert to sRGB
            color.setColorSpace("RGBA", "U8", "sRGB built-in")
            red   = max(min(int(color.componentsOrdered()[0]*255), 255), 0)
            green = max(min(int(color.componentsOrdered()[1]*255), 255), 0)
            blue  = max(min(int(color.componentsOrdered()[2]*255), 255), 0)
            gplFile.write(str(red)+" "+str(green)+" "+str(blue)+"    "+entry.id+"-"+entry.name+"\n")
gplFile.close()
