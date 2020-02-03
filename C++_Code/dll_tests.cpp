#include "doubly_linked_list.h"
#include "gtest/gtest.h"


// Use the TEST macro to define your tests.
//
// TEST has two parameters: the test case name and the test name.
// After using the macro, you should define your test logic between a
// pair of braces.  You can use a bunch of macros to indicate the
// success or failure of a test.  EXPECT_TRUE and EXPECT_EQ are
// examples of such macros.  For a complete list, see gtest.h.
//
// <TechnicalDetails>
//
// In Google Test, tests are grouped into test cases.  This is how we
// keep test code organized.  You should put logically related tests
// into the same test case.
//
// The test case name and the test name should both be valid C++
// identifiers.  And you should not use underscore (_) in the names.
//
// Google Test guarantees that each test you define is run exactly
// once, but it makes no guarantee on the order the tests are
// executed.  Therefore, you should write your tests in such a way
// that their results don't depend on their order.
//
// </TechnicalDetails>


struct ListItem {};

// Don't create spares! It will cause memory leaks
void make_items(ListItem* result[], unsigned n)
{
	while(n--)
	{
		result[n] = (ListItem*) malloc(sizeof(ListItem));
	}
}


TEST(InitializationTests, CreateDestroy)
{
	DLinkedList* list = create_dlinkedlist();
	destroyList(list);
}

TEST(Access, getHead_Empty)
{
	DLinkedList* list = create_dlinkedlist();
	EXPECT_EQ(NULL, getHead(list));
	destroyList(list);
}


TEST(Insert, Head_Single)
{
	// Initialize items for your test
	size_t num_items = 1;
	ListItem* m[num_items]; make_items(m, num_items);

	// Use the items in your list
	DLinkedList* list = create_dlinkedlist();
	insertHead(list, m[0]);

	// Check that the behavior was correct
	EXPECT_EQ(m[0], getHead(list));
	EXPECT_EQ(m[0], getTail(list));

	// Delete the list
	destroyList(list);
}

TEST(Insert, Head_Multiple)
{
	// Create list items
	size_t num_items = 3;
	ListItem* m[num_items]; make_items(m, num_items);

	// Insert 3 items at the head (list is now [2, 1, 0])
	DLinkedList* list = create_dlinkedlist();
	for (int i = 0; i < 3; i++)
		insertHead(list, m[i]);

	// Check forward links
	ASSERT_EQ(m[2], getHead(list));
	for (int i = 1; i >= 0; i--)
		ASSERT_EQ(m[i], getNext(list));
	ASSERT_EQ(NULL, getNext(list));

	// Check backward links
	ASSERT_EQ(m[0], getTail(list));
	for (int i = 1; i < 3; i++)
		ASSERT_EQ(m[i], getPrevious(list));
	ASSERT_EQ(NULL, getPrevious(list));

	// Delete the list
	destroyList(list);
}