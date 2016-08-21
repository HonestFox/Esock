.PHONY:client
client:client.cpp common.cpp transfer.cpp -lpthread
	g++ -o $@ $^

.PHONY:server
server:server.cpp common.cpp transfer.cpp -lpthread
	g++ -o $@ $^

.PHONY:clean
clean:
	rm -f client server
