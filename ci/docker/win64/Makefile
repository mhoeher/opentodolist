IMAGE ?= registry.gitlab.com/rpdev/opentodolist:win64
help:
	@echo "Targets:"
	@echo "  build - Build the image."
	@echo "  publish - Upload the image to GitLab."

build:
	buildah bud --pull -t ${IMAGE} .

publish:
	buildah push docker://${IMAGE}

login:
	buildah login registry.gitlab.com
