#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/XmlOutputter.h>

int main(int argc, char* argv[])
{
	// Retrieve test path from command line first argument. Default to "" which resolve
	// to the top level suite.
	std::string testPath = (argc > 1) ? std::string(argv[1]) : std::string("");
	
	// Create the event manager and test controller
	CPPUNIT_NS::TestResult controller;
	
	// Add a listener that collects test result
	CPPUNIT_NS::TestResultCollector result;
	
	controller.addListener( &result );
	
	// Add a listener that print dots as test run.
	CPPUNIT_NS::BriefTestProgressListener progress;
	
	controller.addListener( &progress );
	
	// Add the top suite to the test runner
	CPPUNIT_NS::TestRunner runner;
	runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
	runner.run( controller );
	
	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write();
	
	// Uncomment this for XML output
	std::ofstream file( "cppunit-report.xml" );
	
	CPPUNIT_NS::XmlOutputter xml( &result, file );
	
	xml.write();
	
	file.close();
	
	return result.wasSuccessful() ? 0 : 1;
}