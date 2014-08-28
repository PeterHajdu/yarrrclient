Given(/^a server listening on port (\d+)$/) do | port |
  @server = TCPServer.new( "127.0.0.1", port )
end

Then(/^the client should connect to the server$/) do
  connection = @server.accept_nonblock
  expect( connection ).not_to be( nil )
end

