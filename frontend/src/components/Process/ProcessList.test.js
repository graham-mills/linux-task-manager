import React from "react";
import { render, screen, act } from "@testing-library/react";
import userEvent from "@testing-library/user-event";
import ProcessList from "./ProcessList";
import { ConnectionStatus } from "../../context/conn-status";
import axios from "axios";
import { Config } from "../../config/config";

jest.useFakeTimers();
jest.mock("axios");

// Advances fake timers by POLL_PERIOD_MS and
// runs any pending promise jobs
const advanceTimeByPollPeriod = async () => {
   await act(async () => {
      jest.advanceTimersByTime(Config.POLL_PERIOD_MS);
      await Promise.resolve();
   });
};

beforeEach(() => {
   axios.get.mockResolvedValue({
      data: [],
   });
});

describe("Memory component", () => {
   test("should not fetch proc snapshots after POLL_PERIOD_MS when conn status is not ok", async () => {
      render(<ProcessList connStatus={ConnectionStatus.Connecting} />);
      await advanceTimeByPollPeriod();

      expect(axios.get).toHaveBeenCalledTimes(0);
   });

   test("should fetch proc snapshots after POLL_PERIOD_MS when conn status is ok", async () => {
      render(<ProcessList connStatus={ConnectionStatus.Ok} />);
      await advanceTimeByPollPeriod();

      expect(axios.get).toHaveBeenCalledTimes(1);
      expect(axios.get).toHaveBeenCalledWith(Config.Endpoints.Procs);
   });

   test("should render processes as list table by default", async () => {
      render(<ProcessList connStatus={ConnectionStatus.Ok} />);

      // The list table is expected to include the `PPID` column
      expect(screen.queryByText("PPID")).not.toBeNull();
   });

   test("should render processes as tree table when corresponding button clicked", async () => {
      render(<ProcessList connStatus={ConnectionStatus.Ok} />);
      await advanceTimeByPollPeriod();

      // Click button labelled `Tree View`
      userEvent.click(screen.getByText("Tree View"));

      // The tree table is expected to not include the `PPID` column
      expect(screen.queryByText("PPID")).toBeNull();
   });

   test("should render processes as list table when corresponding button clicked when viewing processes as tree table", async () => {
      render(<ProcessList connStatus={ConnectionStatus.Ok} />);
      await advanceTimeByPollPeriod();
      userEvent.click(screen.getByText("Tree View"));
      userEvent.click(screen.getByText("List View"));

      // The list table is expected to include the `PPID` column
      expect(screen.queryByText("PPID")).not.toBeNull();
   });
});
