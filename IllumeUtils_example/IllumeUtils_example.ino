#include <ArduinoUnit.h>
#include <IllumeUtils.h>


struct cmd_test_parsed_if_zero_args_passed : TestOnce
{
	cmd_test_parsed_if_zero_args_passed()
		: TestOnce("cmd_test_parsed_if_zero_args_passed")
	{}

	void once()
	{
		String test_data("$test,0");
		StringStream test_stream(test_data);
		struct Command actual_result = readCommand(test_stream);
		assertEqual(actual_result.m_type, CommandType_Test);
		assertEqual(actual_result.m_argc, 0);
		assertTrue(actual_result.m_argv == NULL);
	}
} cmd_test_parsed_if_zero_args_passed_instance;


struct cmd_test_parsed_if_one_args_passed : TestOnce
{
	cmd_test_parsed_if_one_args_passed()
		: TestOnce("cmd_test_parsed_if_one_args_passed")
	{}

	void once()
	{
		String test_data("$test,1,0:0");
		StringStream test_stream(test_data);
		struct Command actual_result = readCommand(test_stream);
		assertEqual(actual_result.m_type, CommandType_Test);
		assertEqual(actual_result.m_argc, 1);
		assertTrue(actual_result.m_argv != NULL);
		assertTrue(actual_result.m_argv[0].equals("0:0"));
	}
} cmd_test_parsed_if_one_args_passed_instance;


struct CmdTestNegativeTestOnce : TestOnce
{
	struct Command m_actualResult;

	CmdTestNegativeTestOnce(const char * test_name)
		: TestOnce(test_name)
	{}

	void once()
	{
		assertEqual(m_actualResult.m_type, CommandType_None);
		assertEqual(m_actualResult.m_argc, -1);
		assertTrue(m_actualResult.m_argv == NULL);
	}
};


struct cmd_test_is_parsed_as_none_if_args_more_than_count : CmdTestNegativeTestOnce
{
	cmd_test_is_parsed_as_none_if_args_more_than_count()
		: CmdTestNegativeTestOnce("cmd_test_is_parsed_as_none_if_args_more_than_count")
	{}

	void once()
	{
		String test_string("$test,abc,bca");
		StringStream test_stream(test_string);
		m_actualResult = readCommand(test_stream);
		CmdTestNegativeTestOnce::once();
	}
} cmd_test_is_parsed_as_none_if_args_more_than_count_instance;


struct cmd_test_is_parsed_as_none_if_args_less_than_count : CmdTestNegativeTestOnce
{
	cmd_test_is_parsed_as_none_if_args_less_than_count()
		: CmdTestNegativeTestOnce("cmd_test_is_parsed_as_none_if_args_less_than_count")
	{}

	void once()
	{
		String test_string("$test,2,0|0");
		StringStream test_stream(test_string);
		m_actualResult = readCommand(test_stream);
		CmdTestNegativeTestOnce::once();
	}
} cmd_test_is_parsed_as_none_if_args_less_than_count_instance;


void setup()
{
	Serial.begin(9600);
	while(!Serial);
}

void loop()
{
	Test::run();
}
