""" OpenFactory-SDK Command Line Interface. """
import click
import openfactory_sdk as sdk


@click.group()
def cli():
    """ OpenFactory SDK CLI - Develop and test OpenFactory apps locally. """
    pass


@cli.group()
def device():
    """ Manage MTConnect devices. """
    pass


@cli.group()
def app():
    """ Manage OpenFactory apps. """
    pass


@cli.group()
def asset():
    """ Manage OpenFactory assets. """
    pass


# Register commands
app.add_command(sdk.app.click_up)
app.add_command(sdk.app.click_down)
app.add_command(sdk.app.click_ls)

asset.add_command(sdk.asset.click_ls)
asset.add_command(sdk.asset.click_inspect)

device.add_command(sdk.device.click_up)
device.add_command(sdk.device.click_down)
device.add_command(sdk.device.click_ls)
