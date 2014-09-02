@smoke
Feature: Starting without a graphical interface
  To be able to test without a graphical engine
  As a test automation engineer
  I should be able to start the client with a terminal only output

  Scenario: starting with --text parameter
    When I plain start yarrr with command line parameter --text --server not.existing.domain:2001
    Then the client should be running

