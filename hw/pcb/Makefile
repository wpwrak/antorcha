#MKMK=../../../cae-tools/cameo/templates/mkmk-simple
MKMK=./mkmk

NAME=antorcha

# 11.5, 0

#BOARD = X0=12.5mm Y0=1mm BOARD_Z=1.6mm CLEARANCE=3mm
BOARD = X0=5mm Y0=1mm BOARD_Z=1.6mm CLEARANCE=3mm

# drill: 12.5-13.5 mil
# in:	 35 mil endmill; pretend it is smaller, to compensate for
#	 tool/board deflections
# out:	 35 mil endmill, 11 mil deflection
#
TOOLS = DRILL=7mil,15mil MILL_IN=12mil MILL_OUT=50mil
TOOLS = DRILL=7mil,15mil MILL_IN=35mil MILL_OUT=35mil ANY=any

#FAB = ROT=90 X=0 Y=1 XN=1 YN=2
FAB = ROT=90 X=0 Y=0 XN=1 YN=1

.PHONY:		all clean

all:		Makefile.mkmk

Makefile.mkmk:	$(MKMK) ../$(NAME).drl ../$(NAME)-PCB_Edges.gbr Makefile
		$(MKMK) NAME=$(NAME) $(BOARD) $(TOOLS) $(FAB)

../$(NAME).drl:	../$(NAME).brd
		pcbnew --drill `pwd`/../$(NAME).brd
		touch $@

../$(NAME)-PCB_Edges.gbr: ../$(NAME).brd
		pcbnew --plot=gerber --layers=PCB_Edges `pwd`/../$(NAME).brd

-include Makefile.mkmk

clean::
		rm -f Makefile.mkmk
