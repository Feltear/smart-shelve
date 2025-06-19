"""
OpenFactory-SDK Command Line Interface.

Usage: openfactory-sdk [OPTIONS] COMMAND [ARGS]...
Help: openfactory-sdk --help

Becomes available after installing OpenFactory-SDK like

> pip install git+https://github.com/Demo-Smart-Factory-Concordia-University/OpenFactory-SDK.git

or (during development, after cloning the repository locally)

> pip install -e .
"""

from openfactory_sdk.sdk_cli import cli
from openfactory.models.user_notifications import user_notify
from openfactory.ofa.ksqldb import ksql
from openfactory.kafka.ksql import KSQLDBClientException
import openfactory.config as Config


def init_environment() -> bool:
    """ Setup OpenFactory environment (notifications, ksqlDB). """
    user_notify.setup(
        success_msg=lambda msg: print(f"{Config.OFA_SUCCSESS}{msg}{Config.OFA_END}"),
        fail_msg=lambda msg: print(f"{Config.OFA_FAIL}{msg}{Config.OFA_END}"),
        info_msg=print
    )

    try:
        ksql.connect(Config.KSQLDB_URL)
    except KSQLDBClientException:
        user_notify.fail('Failed to connect to ksqlDB server. Make sure to spin up the OpenFactory infrastructure.')
        return False

    return True


def ofa_sdk_cli():
    """ Main entry point of openfactory-sdk cli. """
    if not init_environment():
        exit(1)
    cli()


if __name__ == "__main__":
    ofa_sdk_cli()
