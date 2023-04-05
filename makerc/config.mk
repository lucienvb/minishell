#	config.mk

#	Executable	names
MINISHELL								:=minishell.out
UNIT_TEST								:=test.out

#	Main	source
MAIN										:=main.c

#	Source	files
SRCS										:=$(foreach	dir,	$(SRCS_DIRS),	$(wildcard	$(SRC_DIR)/$(dir)/*.c))
UNIT_SRCS								:=$(wildcard	$(UNIT_DIR)/$(SRC_DIR)/*.c)

#	Object	files
OBJS										:=$(patsubst	$(SRC_DIR)/%.c,	$(BUILD_DIR)/%.o,	$(SRCS))
MAIN_OBJ								:=$(addprefix	$(BUILD_DIR)/,	$(MAIN:%.c=%.o))
UNIT_OBJS								:=$(patsubst	$(UNIT_DIR)/$(SRC_DIR)/%.c,	$(UNIT_DIR)/$(BUILD_DIR)/%.o,	$(UNIT_SRCS))

#	Headers
HEADERS									=$(LIBFT_INCLUDE_DIR)/libft.h	\
													$(INCLUDE_DIR)/minishell.h	\
													$(READLINE_INCLUDE_DIR)/readline.h
INCLUDE_FLAGS						:=$(addprefix	-I,	$(sort	$(dir	$(HEADERS))))
UNIT_HEADERS						:=$(wildcard	$(UNIT_DIR)/$(INCLUDE_DIR)/*.h)
UNIT_INCLUDE_FLAGS			:=$(addprefix -I, $(sort $(dir $(UNIT_HEADERS))))

ifeq ($(shell uname -s), Darwin)
    INCLUDE_FLAGS +=$(addprefix -I, $(shell brew --prefix readline)/include)
    LDFLAGS :=-lcriterion -lreadline -L$(shell brew --prefix readline)/lib 
  else
		LDFLAGS :=-lcriterion -lreadline
endif

#	Libraries
LIBFT										:=$(LIBFT_DIR)/libft.a
