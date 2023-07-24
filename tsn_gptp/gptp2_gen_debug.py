/*
 * Copyright (c) 2023 Texas Instruments Incorporated
 * Copyright (c) 2023 Excelfore Corporation (https://excelfore.com)
 *
 * All rights reserved not granted herein.
 * Limited License.
 *
 * Texas Instruments Incorporated grants a world-wide, royalty-free,
 * non-exclusive license under copyrights and patents it now or hereafter
 * owns or controls to make, have made, use, import, offer to sell and sell ("Utilize")
 * this software subject to the terms herein. With respect to the foregoing patent
 * license, such license is granted solely to the extent that any such patent is necessary
 * to Utilize the software alone. The patent license shall not apply to any combinations which
 * include this software, other than combinations with devices manufactured by or for TI ("TI Devices").
 * No hardware patent is licensed hereunder.
 *
 * Redistributions must preserve existing copyright notices and reproduce this license (including the
 * above copyright notice and the disclaimer and (if applicable) source code license limitations below)
 * in the documentation and/or other materials provided with the distribution
 *
 * Redistribution and use in binary form, without modification, are permitted provided that the following
 * conditions are met:
 *
 * * No reverse engineering, decompilation, or disassembly of this software is permitted with respect to any
 * software provided in binary form.
 * * any redistribution and use are licensed by TI for use only with TI Devices.
 * * Nothing shall obligate TI to provide you with source code for the software licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution of the source code are permitted
 * provided that the following conditions are met:
 *
 * * any redistribution and use of the source code, including any resulting derivative works, are licensed by
 * TI for use only with TI Devices.
 * * any redistribution and use of any object code compiled from the source code and any resulting derivative
 * works, are licensed by TI for use only with TI Devices.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of its suppliers may be used to endorse or
 * promote products derived from this software without specific prior written permission.
 *
 * DISCLAIMER.
 *
 * THIS SOFTWARE IS PROVIDED BY TI AND TI"S LICENSORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TI AND TI"S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*/
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
