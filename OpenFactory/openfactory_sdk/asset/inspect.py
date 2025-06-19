""" openfactory-sdk asset inspect command. """

import click
from rich import box
from rich.console import Console
from rich.table import Table
from openfactory.assets import Asset
from openfactory.ofa.ksqldb import ksql


@click.command(name='inspect')
@click.argument('asset_uuid', type=click.STRING)
def click_inspect(asset_uuid: str) -> None:
    """ List all attributes from an asset. """

    console = Console()
    table = Table(
        title=asset_uuid,
        title_justify="left",
        box=box.HORIZONTALS,
        show_lines=True)

    table.add_column("ID", style="cyan", no_wrap=True)
    table.add_column("Value", justify="left")
    table.add_column("Type", justify="left")
    table.add_column("Tag", justify="left")

    asset = Asset(asset_uuid, ksqlClient=ksql.client)

    samples = asset.samples()
    for key in samples:
        table.add_row(key,
                      samples[key],
                      'Sample')

    events = asset.events()
    for key in events:
        table.add_row(key,
                      events[key],
                      'Events')

    conditions = asset.conditions()
    for cond in conditions:
        table.add_row(cond['ID'],
                      cond['VALUE'],
                      'Condition',
                      cond['TAG'])

    console.print(table)
