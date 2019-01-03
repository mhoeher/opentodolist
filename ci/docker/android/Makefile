IMAGE ?= registry.gitlab.com/rpdev/opentodolist:android
help:
	@echo "Targets:"
	@echo "  build - Build the image."
	@echo "  publish - Upload the image to GitLab."

build:
        buildah bud --pull -t ${IMAGE} .

publish:
        buildah push docker://${IMAGE}

login:
        podman login registry.gitlab.com
