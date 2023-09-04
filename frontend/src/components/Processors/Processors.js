import { useEffect, useState } from "react";
import Card from "../UI/Card";
import { Config } from "../../config/config";
import MeterBar from "../UI/MeterBar";
import { Column, FlexTable, Label, Row, Value } from "../UI/FlexTable";
import { ConnectionStatus } from "../../context/conn-status";
import axios from "axios";

const fetchCpuSnapshots = async () => {
   return axios
      .get(Config.Endpoints.Cpus)
      .then((response) => response.data)
      .catch((error) => {
         console.error(error);
      });
};

const Processors = ({ connStatus }) => {
   const [cpuSnapshots, setCpuSnapshots] = useState([]);

   useEffect(() => {
      if (connStatus !== ConnectionStatus.Ok) {
         return;
      }
      const interval = setInterval(() => {
         fetchCpuSnapshots().then((data) => {
            if (data) {
               data.sort((a, b) => (a.id > b.id ? 1 : -1));
               setCpuSnapshots(data);
            }
         });
      }, Config.POLL_PERIOD_MS);
      return () => {
         clearInterval(interval);
      };
   }, [connStatus]);

   const cpuRows = cpuSnapshots.map((cpu) => (
      <Row key={cpu.id}>
         <Label>{cpu.id}</Label>
         <Value>
            <MeterBar fillPercent={`${cpu.usage_percent.toFixed(0)}%`} />
         </Value>
      </Row>
   ));
   // Split the rows in half to be displayed in 2 adjacent columns
   const midpoint = Math.ceil(cpuRows.length / 2);
   const cpuRowsColumn1 = cpuRows.slice(0, midpoint);
   const cpuRowsColumn2 = cpuRows.slice(midpoint, cpuRows.length);

   let totalUsage = 0;
   const aggregateCpu = cpuSnapshots.find((cpu) => cpu.id === "cpu");
   if (aggregateCpu) {
      totalUsage = aggregateCpu.usage_percent;
   }

   return (
      <Card title="Processors">
         <FlexTable columns={2}>
            <Column>
               <Row>
                  <Label>CPUs</Label>
                  <Value>{cpuSnapshots.length}</Value>
               </Row>
            </Column>
            <Column>
               <Row>
                  <Label>Utilization</Label>
                  <Value>{totalUsage.toFixed(2)}%</Value>
               </Row>
            </Column>
         </FlexTable>
         <FlexTable columns={2}>
            <Column>{cpuRowsColumn1}</Column>
            <Column>{cpuRowsColumn2}</Column>
         </FlexTable>
      </Card>
   );
};

export default Processors;
