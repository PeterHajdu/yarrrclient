
@smoke
Feature: Specify loglevel with command line parameter
  To be able to investigate problems with much more detail
  As a yarrr developer
  I should be able to specify the loglevel

  Scenario: specify with command line parameter
    When I start yarrr with a server and command line parameter --loglevel 0
    Then the client should be running
    And I should see Loglevel is set to 0

  Scenario: no debuglog without command line parameter
    Given a running client
    Then the client should be running
    Then I should not see Loglevel is set to

