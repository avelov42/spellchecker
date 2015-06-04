# CMake generated Testfile for 
# Source directory: /home/ryba/workspace/spellcheck/src/dict-editor
# Build directory: /home/ryba/workspace/spellcheck/cbuild/dict-editor
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(dict-editor_global_test "/home/ryba/workspace/spellcheck/src/../tests/dict-editor/test.sh" "/home/ryba/workspace/spellcheck/cbuild/dict-editor/dict-editor")
SET_TESTS_PROPERTIES(dict-editor_global_test PROPERTIES  WORKING_DIRECTORY "/home/ryba/workspace/spellcheck/src/../tests/dict-editor")
