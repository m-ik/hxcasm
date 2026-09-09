#include "gtest/gtest.h"

extern "C" {
#include "parse.h"
}

class ParseTest : public ::testing::Test {
protected:
	void SetUp() override {}
	void TearDown() override {}
};

TEST_F(ParseTest, ParseLabel) {
	uint16_t val;
	struct parsed_line parsed;
	char line[] = "(LOOP)";
	parse(&parsed, line, sizeof(line) - 1);
	EXPECT_EQ(parsed.type, LINE_TYPE_LABEL);
	EXPECT_STREQ(parsed.sym.name, "LOOP");
	EXPECT_FALSE(parsed.sym.is_constant);
}

TEST_F(ParseTest, ParseAInstructionVariable) {
	uint16_t val;
	struct parsed_line parsed;
	char line[] = "@tmp";
	parse(&parsed, line, sizeof(line) - 1);
	EXPECT_EQ(parsed.type, LINE_TYPE_A_INSTR);
	EXPECT_STREQ(parsed.sym.name, "tmp");
	EXPECT_FALSE(parsed.sym.is_constant);
}

TEST_F(ParseTest, ParseAInstructionConstant) {
	uint16_t val;
	struct parsed_line parsed;
	char line[] = "@17";
	parse(&parsed, line, sizeof(line) - 1);
	EXPECT_EQ(parsed.type, LINE_TYPE_A_INSTR);
	EXPECT_STREQ(parsed.sym.name, "17");
	EXPECT_TRUE(parsed.sym.is_constant);
}

TEST_F(ParseTest, ParseCInstructionDestComp) {
	uint16_t val;
	struct parsed_line parsed;
	char line[] = "D=D-M";
	parse(&parsed, line, sizeof(line) - 1);
	EXPECT_EQ(parsed.type, LINE_TYPE_C_INSTR);
	EXPECT_STREQ(parsed.c.dst, "D");
	EXPECT_STREQ(parsed.c.cmp, "D-M");
	EXPECT_TRUE(parsed.c.jmp == NULL);
}

TEST_F(ParseTest, ParseCInstructionCompJump) {
	uint16_t val;
	struct parsed_line parsed;
	char line[] = "0;JMP";
	parse(&parsed, line, sizeof(line) - 1);
	EXPECT_EQ(parsed.type, LINE_TYPE_C_INSTR);
	EXPECT_TRUE(parsed.c.dst == NULL);
	EXPECT_STREQ(parsed.c.cmp, "0");
	EXPECT_STREQ(parsed.c.jmp, "JMP");
}

TEST_F(ParseTest, ParseCInstructionDestCompJump) {
	uint16_t val;
	struct parsed_line parsed;
	char line[] = "ADM=D&M;JLE";
	parse(&parsed, line, sizeof(line) - 1);
	EXPECT_EQ(parsed.type, LINE_TYPE_C_INSTR);
	EXPECT_STREQ(parsed.c.dst, "ADM");
	EXPECT_STREQ(parsed.c.cmp, "D&M");
	EXPECT_STREQ(parsed.c.jmp, "JLE");
}

TEST_F(ParseTest, ParseCInstructionMalformed) {
	uint16_t val;
	struct parsed_line parsed;
	char line[] = "JMP;D=D+1";
	parse(&parsed, line, sizeof(line) - 1);
	EXPECT_EQ(parsed.type, LINE_TYPE_INVALID);
}

TEST_F(ParseTest, ParseCInstructionInvalidDst) {
	uint16_t val;
	struct parsed_line parsed;
	char line[] = "X=D+1;JMP";
	parse(&parsed, line, sizeof(line) - 1);
	EXPECT_EQ(parsed.type, LINE_TYPE_INVALID);
}

TEST_F(ParseTest, ParseCInstructionMissingCmp) {
	uint16_t val;
	struct parsed_line parsed;
	char line[] = "D=;JLE";
	parse(&parsed, line, sizeof(line) - 1);
	EXPECT_EQ(parsed.type, LINE_TYPE_INVALID);
}

TEST_F(ParseTest, ParseCInstructionInvalidJmp) {
	uint16_t val;
	struct parsed_line parsed;
	char line[] = "D=D+1;JMP;JMP";
	parse(&parsed, line, sizeof(line) - 1);
	EXPECT_EQ(parsed.type, LINE_TYPE_INVALID);
}
