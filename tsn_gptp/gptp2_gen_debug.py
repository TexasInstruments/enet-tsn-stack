#!/usr/bin/env python3
from __future__ import print_function
import sys, os, re

class oneEnumItem(object):
    def __init__(self, fname, smark, emark):
        self.inf=open(fname,"r")
        if smark and emark: self.scanfile(smark, emark)
        regexp1=r"(\S[^\s^,^=]*)"
        regexp2=r"(\S[^\s^,^=]*) *= *([0-9]*)"
        self.item_num = self.item_list(regexp1, regexp2)

    def scanfile(self, smark, emark):
        state=0
        self.linebuf=[]
        while True:
            line=self.inf.readline()
            if line=="": break
            if state==0:
                if line.find(smark)>=0: state=1
                continue
            if state==1:
                if line.find(smark)>=0:
                    self.linebuf=[]
                    continue
                if line.find(emark)>=0: break
                self.linebuf.append(line)
                continue
        self.inf.close()

    def item_list(self, regstr1, regstr2 , prefix='"', suffix='"'):
        rea1=re.compile(regstr1)
        rea2=re.compile(regstr2)
        self.items=[]
        for line in self.linebuf:
            ro=rea2.search(line)
            if ro:
                self.items.append(("%s%s%s" % (prefix, ro.group(1), suffix),int(ro.group(2))))
                continue;
            ro=rea1.search(line)
            if not ro: continue
            self.items.append(("%s%s%s" % (prefix, ro.group(1), suffix),-1))
        return self.print_items(None, None, False);

    def print_items(self, first_line, last_line="};", pr=True):
        if pr: print(first_line)
        j=0
        for i in self.items:
            if i[1]!=-1 and j!=i[1]:
                if pr:
                    for k in range(i[1]-j): print("	\"\",")
                j=i[1]+1
            else:
                j=j+1
            if pr: print("	%s," % i[0])
        if pr: print(last_line)
        if pr: print
        return j

if __name__ == '__main__':
    header_mode=False
    if len(sys.argv)>1 and sys.argv[1]=="h":
        header_mode=True

    items_list=[["mdeth.h", "PTPMsgType",
                 ["enum","PTPMsgType", "char *PTPMsgType_debug"]],
                ["gptpnet.h", "gptpnet_event_t",
                 ["enum", "gptpnet_event", "char *gptpnet_event_debug"]],
                ["gptpbasetypes.h", "PTPPortState",
                 ["enum", "PTPPortState", "char *PTPPortState_debug"]],
    ]

    print("/* Automatically generated code. Don't edit this file. */")
    print()

    for item in items_list:
        if header_mode:
            print("extern %s[];" % item[2][2])
            continue
        a=oneEnumItem(item[0], item[2][0], item[2][1])
        a.print_items("%s[%d] = {" % (item[2][2],a.item_num))
