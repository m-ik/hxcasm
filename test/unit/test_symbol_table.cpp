#include "gtest/gtest.h"

extern "C" {
#include "symbols.h"
}

class SymbolTableTest : public ::testing::Test {
    protected:
	void SetUp() override
	{
		symbol_table = symbol_table_init();
	}
	void TearDown() override
	{
		symbol_table_destroy(symbol_table);
	}

	struct hashtable *symbol_table;
};

TEST_F(SymbolTableTest, SymbolTableLookup)
{
	struct symbol *sym = symbol_table_lookup(symbol_table, "R5");
	EXPECT_TRUE(sym);
	EXPECT_EQ(sym->val, 0x0005);
}

TEST_F(SymbolTableTest, SymbolTableInsert)
{
	EXPECT_TRUE(symbol_table_insert(symbol_table, "tmp", 42));
	struct symbol *sym = symbol_table_lookup(symbol_table, "tmp");
	EXPECT_TRUE(sym);
	EXPECT_EQ(sym->val, 42);
}
