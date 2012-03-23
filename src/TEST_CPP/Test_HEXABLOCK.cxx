//  Copyright (C) 2009-2011  CEA/DEN, EDF R&D
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/XmlOutputter.h>

#include "test_unit.hxx"

#define TestUnit(call) cout << barre << " .... Essai de " #call << endl; \
                       call(nbargs,tabargs) ; if (ier!=0) return ier
// ============================================================== Main
int main (int nbargs, cpchar tabargs [])
{
   int ier;
   cpchar barre = "=====================================================";

   TestUnit (test_sphere );
   TestUnit (test_joint );
   TestUnit (test_prism );
   TestUnit (test_revolution9 );
   TestUnit (test_revolution );
   TestUnit (test_coude );
   TestUnit (test_count );
   TestUnit (test_decoupage );
   TestUnit (test_gen_xml );
   TestUnit (test_string_xml );
   ///   TestUnit (test_relecture );
   TestUnit (test_spherical);
   TestUnit (test_grille_cyl );
   TestUnit (test_asso_line );
   TestUnit (test_cylindrical );
   TestUnit (test_cylinder );
   TestUnit (test_xml_cylinder );
   TestUnit (test_pipe );
   TestUnit (test_joint2 );
   TestUnit (test_croix );
   TestUnit (test_pipes );
   TestUnit (test_lorraine);
   TestUnit (test_disconnect2 );
   TestUnit (test_disconnect4 );
   TestUnit (test_disconnect1 );
   TestUnit (test_disconnect3 );
   TestUnit (test_disconnect );
   TestUnit (test_transfo2 );
   TestUnit (test_transfo );
   TestUnit (test_copy_document );
   TestUnit (test_cylindricals );
   TestUnit (test_hexa );
                 // Dans test_quads
   TestUnit (test_hexa_quads_5 );
   TestUnit (test_hexa_quads_ab );
   TestUnit (test_hexa_quads_ac );
   TestUnit (test_hexa_quads_ace );
   TestUnit (test_hexa_quads_acd );
   TestUnit (test_hexa_quads_abcd );
   TestUnit (test_hexa_quads_abce );
   TestUnit (test_cramer );
   TestUnit (test_hexa_quads_ac1 );
   TestUnit (test_asso_grid );
   TestUnit (test_piquage );
   TestUnit (test_replace );
   TestUnit (test_hemispheres );
   TestUnit (test_quads );
}
// ============================================================== main-type
int main_standard (int argc, char* argv[])
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
