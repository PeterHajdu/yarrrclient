@smoke
Feature: Print out meaningful information to the terminal
  To be able to start the binary as easily as possible
  As an aspiring yarrr player
  I should be able to see meaningful information in the terminal

  Scenario: starting with --help parameter
    When I start yarrr with command line parameter --help
    Then the help message should be on the screen

