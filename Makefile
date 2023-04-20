include makerc/common.mk
include makerc/config.mk

-include $(UNIT_DEPENDS)
-include $(DEPENDS) 
-include $(MAIN_DEPENDS)

all: $(MINISHELL)

unit_test: $(UNIT_TEST)

$(MINISHELL): SHELL :=/bin/bash

$(MINISHELL): $(OBJS) $(MAIN_OBJ) $(LIBFT)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $(INCLUDE_FLAGS) -o $(MINISHELL)

$(UNIT_TEST): $(UNIT_OBJS) $(OBJS) $(MAIN_OBJ) $(LIBFT)
	$(CC) $(CFLAGS) $(UNIT_OBJS) $(OBJS) $(LDFLAGS) $(UNIT_INCLUDE_FLAGS) $(INCLUDE_FLAGS) $(LIBFT) -o $(UNIT_TEST)

$(MAIN_OBJ) $(OBJS): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(UNIT_OBJS): $(UNIT_BUILD_DIR)/%.o: $(UNIT_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< $(UNIT_INCLUDE_FLAGS) $(INCLUDE_FLAGS) -o $@

$(LIBFT):
	@$(MAKE) -C $(LIBFT_DIR)

debug:
	@$(MAKE) DEBUG=1 LOG=1

fsan:
	@$(MAKE) FSAN=1 DEBUG=1

test:
	@$(MAKE) DEBUG=1 FSAN=1 fsan unit_test
	@./$(UNIT_TEST) -j4 $(F)

coverage:
	@$(RM) $(COVERAGE_GCDA) $(COVERAGE_FILES)
	@$(MAKE) DEBUG=1 COV=1 unit_test
	@./$(UNIT_TEST) -j4 $(F)
	@lcov -q -d build -d unit_test/build -c --output-file build/coverage.info --rc lcov_branch_coverage=1
	@genhtml -q build/coverage.info -o build/coverage_report --rc genhtml_branch_coverage=1

analyse:
	w3m build/coverage_report/index.html

clean:
	@$(RM) $(BUILD_DIR) $(UNIT_BUILD_DIR)
	@$(MAKE) clean -C $(LIBFT_DIR)

fclean: clean
	@$(RM) $(MINISHELL) $(UNIT_TEST)
	@$(MAKE) fclean -C $(LIBFT_DIR)

re: fclean all

resan: fclean fsan

rebug: fclean debug

test_re: fclean test

coverage_re: fclean coverage

bonus: all

.PHONY: all clean fclean re bonus
.PHONY: test test_re 
.PHONY: coverage coverage_re analyse
.PHONY: debug rebug fsan resan
