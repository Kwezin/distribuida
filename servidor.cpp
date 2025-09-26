#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <numeric>

#include <grpcpp/grpcpp.h>
#include "notas.grpc.pb.h"

// Banco em memória (chave: ra_coddisc)
std::map<std::string, gerencia_notas::Nota> db_notas_cpp;

class GerenciadorNotasImpl final : public gerencia_notas::GerenciadorNotas::Service {
public:
    grpc::Status AdicionarNota(grpc::ServerContext* context,
                               const gerencia_notas::AdicionaNotaRequest* request,
                               gerencia_notas::StatusResponse* response) override {
        std::string chave = request->ra() + "_" + request->cod_disciplina();
        if (db_notas_cpp.count(chave)) {
            response->set_sucesso(false);
            response->set_msg("Nota já existe. Use 'AlterarNota'.");
        } else {
            gerencia_notas::Nota nova_nota;
            nova_nota.set_ra(request->ra());
            nova_nota.set_cod_disciplina(request->cod_disciplina());
            nova_nota.set_ano(request->ano());
            nova_nota.set_semestre(request->semestre());
            nova_nota.set_nota(request->nota());
            db_notas_cpp[chave] = nova_nota;
            response->set_sucesso(true);
            response->set_msg("Nota adicionada com sucesso!");
        }
        return grpc::Status::OK;
    }

    grpc::Status AlterarNota(grpc::ServerContext* context,
                             const gerencia_notas::AdicionaNotaRequest* request,
                             gerencia_notas::StatusResponse* response) override {
        std::string chave = request->ra() + "_" + request->cod_disciplina();
        if (!db_notas_cpp.count(chave)) {
            response->set_sucesso(false);
            response->set_msg("Nota não existe. Use 'AdicionarNota'.");
        } else {
            gerencia_notas::Nota &nota = db_notas_cpp[chave];
            nota.set_ano(request->ano());
            nota.set_semestre(request->semestre());
            nota.set_nota(request->nota());
            response->set_sucesso(true);
            response->set_msg("Nota alterada com sucesso!");
        }
        return grpc::Status::OK;
    }

    grpc::Status ConsultarNota(grpc::ServerContext* context,
                               const gerencia_notas::AlunoDisciplinaRequest* request,
                               gerencia_notas::ConsultaNotaResponse* response) override {
        std::string chave = request->ra() + "_" + request->cod_disciplina();
        if (!db_notas_cpp.count(chave)) {
            response->set_sucesso(false);
            response->set_msg_erro("Nota não encontrada");
        } else {
            response->set_sucesso(true);
            *(response->mutable_nota()) = db_notas_cpp[chave];
        }
        return grpc::Status::OK;
    }

    grpc::Status CalcularMedia(grpc::ServerContext* context,
                               const gerencia_notas::AlunoRequest* request,
                               gerencia_notas::MediaResponse* response) override {
        std::string ra = request->ra();
        std::vector<float> notas;
        for (const auto& kv : db_notas_cpp) {
            const gerencia_notas::Nota &n = kv.second;
            if (n.ra() == ra) notas.push_back(n.nota());
        }
        if (notas.empty()) {
            response->set_sucesso(false);
            response->set_msg_erro("Aluno sem notas");
            response->set_media(0.0);
        } else {
            float soma = 0.0f;
            for (float v : notas) soma += v;
            response->set_sucesso(true);
            response->set_media(soma / notas.size());
        }
        return grpc::Status::OK;
    }

    grpc::Status ListarNotasAluno(grpc::ServerContext* context,
                                  const gerencia_notas::AlunoRequest* request,
                                  grpc::ServerWriter<gerencia_notas::Nota>* writer) override {
        std::string ra = request->ra();
        for (const auto& kv : db_notas_cpp) {
            const gerencia_notas::Nota &n = kv.second;
            if (n.ra() == ra) {
                writer->Write(n);
            }
        }
        return grpc::Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50052");
    GerenciadorNotasImpl service;
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Servidor C++ escutando em " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}
