
When(/^I start yarrr with command line parameter (.*)$/) do | parameter |
  @output=`yarrrclient #{parameter}`
end

Then(/^the help message should be on the screen$/) do
  steps %Q{
    Then I should see yarrrthegame.com
    And I should see info@yarrrthegame.com
    And I should see yarrrclient --server <server:port>
  }
end

Then(/^I should see (.+)$/) do | pattern |
  expect( @output ).to match( /#{pattern}/ )
end

