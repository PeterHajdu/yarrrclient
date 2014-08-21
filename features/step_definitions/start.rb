
When(/^I start yarrr with command line parameter (.*)$/) do | parameter |
  @output=`yarrrclient #{parameter}`
end

Then(/^I should see (.+)$/) do | pattern |
  expect( @output ).to match( /#{pattern}/ )
end

