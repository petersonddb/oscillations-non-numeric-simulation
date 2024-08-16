//
//  ContentView.swift
//  VisualizationForMac
//
//  Created by Peterson Bem on 12/03/24.
//

import SwiftUI
import Charts

let dataFileName = "oscillation.dat"

struct ContentView: View {
    struct Oscillation: Identifiable {
        var t: Double
        var x: Double
        var E: Double
        var id = UUID()
    }
    
    @State private var data: [Oscillation] = []

    var body: some View {
        Chart(data) {
            LineMark(
                x: .value("Time (s)", $0.t),
                y: .value("Displacement (m)", $0.x)
            ).foregroundStyle(by: .value("Displacement", "x"))

            LineMark(
                x: .value("Time (s)", $0.t),
                y: .value("Displacement (m)", $0.E)
            ).foregroundStyle(by: .value("Energy", "e"))
        }
        .padding()
        .onAppear(perform: fetchData) // TODO: move this to a progress-only view (avoids rerenderings?)
    }
    
    private func fetchData() {
        var data: [Oscillation] = []
        
        // TODO: change directory from downloads to somewhere inside the app bundle
        if let dir = FileManager.default.urls(for: .downloadsDirectory, in: .userDomainMask).first {
            let dataURL = dir.appendingPathComponent(dataFileName)

            do {
                let text = try String(contentsOf: dataURL, encoding: .utf8)

                for line in text.split(whereSeparator: { $0.isNewline }) {
                    let values = line.split(whereSeparator: { $0.isWhitespace })
                    let entry = Oscillation(
                        t: Double(values[0]) ?? 0, x: Double(values[1]) ?? 0, E: (Double(values[2]) ?? 0))

                    data.append(entry)
                }
            } catch {
                print("could NOT read data file: \(error)!")
            }
        }

        self.data = data
    }
}

#Preview {
    ContentView()
}
