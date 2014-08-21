
When(/^I start yarrr with command line parameter (.*)$/) do | parameter |
  @output=`yarrrclient #{parameter}`
end

Then(/^I should see (.+)$/) do | pattern |
  @output.expect =~/pattern/
end

