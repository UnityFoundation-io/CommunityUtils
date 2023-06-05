import os
import tempfile
import requests
import pandas as pd
from click.testing import CliRunner
from pump import pump_data

dpm_gid = os.getenv('DPM_GID')
base_url = os.getenv('DPD_URL')

ORGANIZATIONS = "organization"
SERVICES = "service"
LOCATIONS = "location"


def test_help():
    runner = CliRunner()
    result = runner.invoke(pump_data, ['--help'])
    assert result.exit_code == 0


def test_import_organizations():
    runner = CliRunner()
    tmp_file = create_temp_file(ORGANIZATIONS, True)
    result = runner.invoke(pump_data, ['-o', tmp_file.name, '--url', base_url])
    if result.exit_code != 0:
        print("OUTPUT: \n" + result.output + "\n")
    os.remove(tmp_file.name)
    drop_all_entities(ORGANIZATIONS)
    assert result.exit_code == 0

    tmp_file = create_temp_file(ORGANIZATIONS, False)
    result = runner.invoke(pump_data, ['-o', tmp_file.name, '--url', base_url])
    if result.exit_code != 0:
        print("OUTPUT: \n" + result.output + "\n")
    os.remove(tmp_file.name)
    drop_all_entities(ORGANIZATIONS)
    assert result.exit_code == 0


def test_import_locations():
    runner = CliRunner()
    tmp_file = create_temp_file(LOCATIONS, True)
    result = runner.invoke(pump_data, ['-l', tmp_file.name, '--url', base_url])
    if result.exit_code != 0:
        print("OUTPUT: \n" + result.output + "\n")
    os.remove(tmp_file.name)
    drop_all_entities(LOCATIONS)
    assert result.exit_code == 0

    tmp_file = create_temp_file(LOCATIONS, False)
    result = runner.invoke(pump_data, ['-l', tmp_file.name, '--url', base_url])
    if result.exit_code != 0:
        print("OUTPUT: \n" + result.output + "\n")
    os.remove(tmp_file.name)
    drop_all_entities(LOCATIONS)
    assert result.exit_code == 0


def test_import_services():
    runner = CliRunner()
    tmp_file = create_temp_file(SERVICES, True)
    result = runner.invoke(pump_data, ['-s', tmp_file.name, '--url', base_url])
    if result.exit_code != 0:
        print("OUTPUT: \n" + result.output + "\n")
    os.remove(tmp_file.name)
    drop_all_entities(SERVICES)
    assert result.exit_code == 0


    tmp_file = create_temp_file(SERVICES, False)
    result = runner.invoke(pump_data, ['-s', tmp_file.name, '--url', base_url])
    if result.exit_code != 0:
        print("OUTPUT: \n" + result.output + "\n")
    os.remove(tmp_file.name)
    drop_all_entities(SERVICES)
    assert result.exit_code == 0


def create_temp_file(entity_type, is_csv):
    if entity_type == ORGANIZATIONS:
        df = get_organizations()
    elif entity_type == LOCATIONS:
        df = get_locations()
    elif entity_type == SERVICES:
        df = get_services()
    else:
        raise ValueError("Only CSV and Excel files are supported.")

    if is_csv:
        tmp_file = tempfile.NamedTemporaryFile(suffix=".csv", delete=False)
        df.to_csv(tmp_file.name, index=False)
    else:
        tmp_file = tempfile.NamedTemporaryFile(suffix=".xlsx", delete=False)
        df.to_excel(tmp_file.name, index=False)

    tmp_file.close()
    return tmp_file


def get_organizations():
    columns = ["name", "description", "id", "alternate_name", "services", "email", "phones", "url", "year_incorporated",
               "tax_status", "tax_id", "legal_status", "locations", "contacts", "programs"]
    data = [
        ["Acme Inc.", "Acme Products are made from the finest of materials!", "abc123", None, None, None, None, None,
         None, None, None, None, None, None, None]  # minimal example
    ]
    return pd.DataFrame(data, columns=columns)


def get_locations():
    columns = ["name", "id", "services", "address", "organization", "alternate_name", "description", "transportation",
               "latitude", "longitude", "phones", "schedule"]
    data = [
        ["St. Louis Greater Area", "stl-ga", None, None, None, None, None, None, None, None, None, None]
    ]
    return pd.DataFrame(data, columns=columns)


def get_services():
    columns = ["name", "id", "alternative_name", "description", "url", "email", "status", "interpretation_services",
               "application_process", "wait_time", "fees", "accreditations", "licenses", "programs", "organizations",
               "locations", "address", "taxonomy", "phones", "schedule", "contacts", "organization_id"]
    data = [
        ["Acme Healthcare", "acme-health", None, "None", "https://sarapis.org/services/", None, None, None, None, None, None, None, None, None, None,
         None, None, None, None, None, None, "abc123"]
    ]
    return pd.DataFrame(data, columns=columns)


def drop_all_entities(entity):
    requests.delete(base_url + "/hsds3/" + entity)


if __name__ == '__main__':
    assert dpm_gid is not None
    assert base_url is not None
    test_help()
    test_import_organizations()
    test_import_locations()
    test_import_services()
