include psn00bsdk-setup.mk

# Project target name
TARGET		= doukutsu

# Searches for C, C++ and S (assembler) files in specified directory
SRCDIR		= src
SRCSUB		= engine game game/npc_act

SRCDIRS		= $(foreach dir,$(SRCSUB),$(SRCDIR)/$(dir)) $(SRCDIR)
CFILES		= $(foreach dir,$(SRCDIRS),$(patsubst $(SRCDIR)/%.c,%.c,$(wildcard $(dir)/*.c)))
CPPFILES 	= $(foreach dir,$(SRCDIRS),$(patsubst $(SRCDIR)/%.cpp,%.cpp,$(wildcard $(dir)/*.cpp)))
AFILES		= $(foreach dir,$(SRCDIRS),$(patsubst $(SRCDIR)/%.s,%.s,$(wildcard $(dir)/*.s)))

# Create names for object files
OFILES		= $(addprefix build/,$(CFILES:.c=.o)) \
			$(addprefix build/,$(CPPFILES:.cpp=.o)) \
			$(addprefix build/,$(AFILES:.s=.o))

# Project specific include and library directories
# (use -I for include dirs, -L for library dirs)
INCLUDE	 	+=
LIBDIRS		+=

# Libraries to link
LIBS		= -lpsxgpu -lpsxspu -lpsxetc -lpsxapi -lpsxcd -lc

# C compiler flags
CFLAGS		:= -g -O3 -fno-builtin -fdata-sections -ffunction-sections -I$(SRCDIR)
CFLAGS		+= -march=mips1 -mabi=32 -EL -fno-pic -mno-shared -mno-abicalls -mfp32

# C++ compiler flags
CPPFLAGS	= $(CFLAGS) -fno-exceptions

# Assembler flags
AFLAGS		= -g

# Linker flags (-Ttext specifies the program text address)
LDFLAGS		= -g -Ttext=0x80010000 -gc-sections \
			-T $(GCC_BASE)/$(PREFIX)/lib/ldscripts/elf32elmip.x

all: $(TARGET).exe

iso: $(TARGET).iso

$(TARGET).iso: $(TARGET).exe
	mkpsxiso -y -q iso.xml

$(TARGET).exe: $(OFILES)
	$(LD) $(LDFLAGS) $(LIBDIRS) $(OFILES) $(LIBS) -o $(TARGET).elf
	elf2x -q $(TARGET).elf

build/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

build/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(AFLAGS) $(INCLUDE) -c $< -o $@

build/%.o: $(SRCDIR)/%.s
	@mkdir -p $(dir $@)
	$(CC) $(AFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -rf build $(TARGET).elf $(TARGET).exe

.PHONY: all iso clean
