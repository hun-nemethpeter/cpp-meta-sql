EXE_FILE=meta_sql

CXX=g++
CXXFLAGS_COMMON = -g -Wall -std=c++11 -ftemplate-depth-20000
CXXFLAGS_INCLUDES = -I. -include pre.h
CXXFLAGS = $(CXXFLAGS_COMMON) $(CXXFLAGS_INCLUDES)
#LDFLAGS = -lboost_regex -lboost_date_time

#nothing to be set manually from here

OBJ_DIR = obj
SRC_FILES:=$(wildcard *.cc)
OBJ_FILES:=$(SRC_FILES:.cc=.o)
OBJ_FILES_WITH_DIR:=$(addprefix $(OBJ_DIR)/, $(OBJ_FILES))

DEP_FILES:=$(SRC_FILES:.cc=.d)
DEP_FILES_WITH_DIR:=$(addprefix $(OBJ_DIR)/, $(DEP_FILES))

NODEPS:=clean

# Add .d to Make's recognized suffixes.
#SUFFIXES += .d

.PHONY: init_rules run_rules
all: init_rules run_rules

init_rules:
	@echo Start
	@date +%s > time

run_rules: $(EXE_FILE)
	@./timediff.sh

clean:
	rm -rf obj
	rm -f $(EXE_FILE) core time dep.out pre.h.gch time

#This is the rule for creating the dependency files
$(DEP_FILES_WITH_DIR): $(SRC_FILES)
	@mkdir -p obj # hack
	$(CXX) -MM -MT $(@:.d=.o) $(notdir $(@:.d=.cc)) -o $@

$(OBJ_FILES_WITH_DIR): pre.h.gch
	$(CXX) $(CXXFLAGS) -c $(notdir $(@:.o=.cc)) -o $@

$(EXE_FILE): $(OBJ_FILES_WITH_DIR)
	$(CXX) $(CXXFLAGS) $(OBJ_FILES_WITH_DIR) -o $@ $(LDFLAGS)

pre.h.gch : pre.h
	$(CXX) $(CXXFLAGS_COMMON) -o $@ $<

.PHONY: MakeTimeStamp
MakeTimeStamp:
	@echo DDDD

#Don't create dependencies when we're cleaning, for instance
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
-include $(DEP_FILES_WITH_DIR)
endif
