IMAGE ?= registry.gitlab.com/rpdev/opentodolist:win32
help:
	@echo "Targets:"
	@echo "  build - Build the image."
	@echo "  publish - Upload the image to GitLab."

build:
	docker build --pull -t ${IMAGE} .

publish:
	docker push ${IMAGE}

login:
	docker login registry.gitlab.com

