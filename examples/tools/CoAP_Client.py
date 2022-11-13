import random
import asyncio

from aiocoap import *

async def main():
	context = await Context.create_client_context()
	Payload = b"OFF"

	if(random.choice([True, False])):
		Payload = b"ON"

	request = Message(code = GET, payload = Payload, uri = "coap://h2907213.stratoserver.net/counter")

	response = await context.request(request).response
	print("Result: %s\n%r"%(response.code, response.payload))

if(__name__ == "__main__"):
	asyncio.get_event_loop().run_until_complete(main())