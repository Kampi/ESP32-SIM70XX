import datetime
import logging

import asyncio

import aiocoap
import aiocoap.resource as resource

class Status(resource.Resource):
	async def render_get(self, request):
		text = ["Used protocol: %s." % request.remote.scheme]

		print("Code: {}".format(request.code))
		print("Token: {}".format(request.token))
		print("Options: {}".format(request.opt.encode()))
		for Option in request.opt.option_list():
			print("	{}".format(Option))

		text.append("Request came from %s." % request.remote.hostinfo)
		text.append("The server address used %s." % request.remote.hostinfo_local)

		claims = list(request.remote.authenticated_claims)
		if claims:
			text.append("Authenticated claims of the client: %s." % ", ".join(repr(c) for c in claims))
		else:
			text.append("No claims authenticated.")

		return aiocoap.Message(content_format = 0, payload = "\n".join(text).encode("utf8"))

	async def render_put(self, request):
		print("PUT payload: %s" % request.payload)
		return aiocoap.Message(code = aiocoap.CHANGED, payload = request.payload)

logging.basicConfig(level = logging.INFO)
logging.getLogger("coap-server").setLevel(logging.DEBUG)

async def main():
	root = resource.Site()
	root.add_resource(["counter"], Status())
	await aiocoap.Context.create_server_context(root)
	await asyncio.get_running_loop().create_future()

if(__name__ == "__main__"):
	asyncio.run(main())