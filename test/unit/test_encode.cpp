#include "gtest/gtest.h"

extern "C" {
#include "encode.h"
#include "symbols.h"
}

class EncodeTest : public ::testing::Test {
protected:
	void SetUp() override {
		symbol_table = symbol_table_init();
	}
	void TearDown() override {
		hashtable_destroy(symbol_table);
	}

	struct hashtable *symbol_table;
};

TEST_F(EncodeTest, EncodeCInstruction) {
	struct parsed_line parsed = {
		.type = LINE_TYPE_C_INSTR,
		.c = {
			.dst = "D",
			.cmp = "D-1",
			.jmp = "JLE",
		},
	};
	uint16_t code = encode(&parsed);
	EXPECT_EQ(code, 0xe396);
}
