
When(/^I start yarrr with command line parameter (.*)$/) do | parameter |
  @yarrr_client = ProcessRunner.new(
    { "SDL_VIDEODRIVER" => "dummy" },
    "yarrrclient #{parameter} 2>&1" )
  @yarrr_client.start
end

Then(/^the help message should be on the screen$/) do
  steps %Q{
    Then I should see http://yarrrthegame.com
    And I should see info@yarrrthegame.com
    And I should see yarrrclient --server <server:port>
  }
end

Then(/^I should see (.+)$/) do | pattern |
  expect( @yarrr_client.output ).to match( /#{pattern}/ )
end

Then(/^I should not see (.+)$/) do | pattern |
  expect(  @yarrr_client.output ).not_to match( /#{pattern}/ )
end

Then(/^there should be no failed assertion$/) do
  expect(  @yarrr_client.output ).not_to match( /assert/i )
end

Then(/^the client should be running$/) do
  expect( @yarrr_client.is_running ).to be true
end

