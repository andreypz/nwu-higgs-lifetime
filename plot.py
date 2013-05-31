#!/usr/bin/env python

import sys,os
import datetime
from optparse import OptionParser
from ROOT import *
gROOT.SetBatch(True)

parser = OptionParser(usage="usage: %prog [options]")
parser.add_option("--s", dest="s", action="store_true", default=False, help="s")
(options, args) = parser.parse_args()

imgpath = "/uscms_data/d2/andreypz/html/lifetime/"


def makeHTML(htmlDir):

    print "\n\n ******** Now making HTML pages ******** \n"
    menu=""
    plot_types = ["4mu","4e"]

    fileList = {}
    for x in plot_types:
        newDir = htmlDir+"/"+x
        if not os.path.exists(newDir):
            print "Creating ", newDir
            os.makedirs(newDir)


        fname = x+".html"
        imgfile = open(fname,"w")
        imgfile.write("<html><head><title>"+x+"</title></head><body>\n")

        fileList[x] = os.listdir(newDir)
        #print fileList[x]

        count =1
        mod =1
        for pl in  sorted(fileList[x]):
            mod = count % 2
            if mod==1: imgfile.write('<nobr><img src='+x+'/'+pl+' width=45%>')
            if mod==0: imgfile.write('      <img src='+x+'/'+pl+' width=45%></nobr>\n')
            count+=1
        if mod==0: imgfile.write("")
        if mod==1: imgfile.write("</nobr>")

        imgfile.write("</body></html>")
        imgfile.close()
        os.system("mv "+fname+" "+htmlDir)

        menu = menu+"<li><a href=\""+x+".html\" target=\"iframe_a\">"+x+"</a></li>"

    today = datetime.date.today()
    print today

    message = '<h2>Comments</h2>\
<ul>\
<li>These plots are from <font color="blue">data</font> sample with m4l in [110,170] window</li>\
<li></li>\
<li></li>\
</ul>'

    tempfile = open("indextemplate.html","r")
    whole_thing = tempfile.read()
    whole_thing = whole_thing.replace("{MENU}", menu)
    whole_thing = whole_thing.replace("{DATE}", str(today))
    whole_thing = whole_thing.replace("{ASIDEMESSAGE}", str(message))
    tempfile.close()


    ifile = open(htmlDir+"/index.html","w")
    ifile.write(whole_thing)
    ifile.close()

    print "\n\n *** End of  making HTML pages - all done *** \n"


def makeTable(table, opt="tex"):
    print "Making sure that the list is alright"
    n_row = len(table)
    n_col = len(table[0])
    for l in table:
        if len(l)!=n_col:
            print "No good, the number of columns is messed up"

    myTable = ''
    if opt=="tex":
        beginTable = '\\begin{tabular}{|'+n_col*"l|"+'} \n \\hline \n'
        endTable   = '\\end{tabular} \n'

        beginLine  = ''
        endLine    = '\\\\\\hline \n'
        separator  = ' & '


    if opt=="html":
        beginTable = '<table border = "10"    cellpadding="5">'
        endTable = '</table>'

        beginLine  = '\n<tr>\n<td>'
        endLine    = '</td>\n</tr>'
        separator  = '</td><td>'

    if opt=="twiki":
        beginTable = ''
        endTable   = ''

        beginLine  = '| '
        endLine    = ' |\n'
        separator  = ' |  '


    myTable +=beginTable
    for l in range(n_row):
        print l, table[l]
        myTable+=beginLine
        for c in range(n_col):
            val = table[l][c]
            if not isinstance(val,str):
                myTable+="%.2f" % (table[l][c])
            else:
                myTable+=val
            if c!=n_col-1:
                myTable+=separator
        myTable+=endLine

    myTable +=endTable

    print myTable


def simpleHist(plotname, hname, path, f1):

    h = f1.Get(hname)
    h.Draw()
    c1.SaveAs(path+hname+"_"+plotname+".png")
    

if __name__ == "__main__":
    #gROOT.ProcessLine(".L     ~/Dropbox/tdrstyle.C")
    #setTDRStyle()
    TH1.SetDefaultSumw2(kTRUE)

    f1 = TFile("output.root","open")

    f1.Print()
    path = imgpath+"4mu/"
    if not os.path.exists(path):
        os.makedirs(path)

    simpleHist("mu_01","nMuons",   path,f1)
    simpleHist("mu_02","nMuons7",  path,f1)
    simpleHist("mu_03","mu_dRmin", path,f1)
    simpleHist("mu_04","mu_dPt",   path,f1)

    simpleHist("tr_","nTracks",   path,f1)

    makeHTML(imgpath)
    
