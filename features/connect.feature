@smoke
Feature: Connects to server
  To start playing
  As an aspiring yarrr player
  I should be able to define the server address

  Scenario: starting with correct server address
    Given a server listening on port 5999
    When I start yarrr with command line parameter --server localhost:5999
    Then the client should be running
    And the client should connect to the server

